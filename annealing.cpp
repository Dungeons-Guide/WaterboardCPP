//
// Created by syeyoung on 8/9/24.
//

#include <iostream>

#include <random>


#include "annealing.h"



bool isDone(Node state[HEIGHT][WIDTH], int* flips, int flipSize, int targetSize, std::vector<Point>& nonTargets) {
    for (int i = 0; i < flipSize; i++) {
        if ((flips[i] % 2 == 1) != (i < targetSize)) {
            return false;
        }
    }
    Node newstate[HEIGHT][WIDTH];
    clone(state, newstate);
    for (int i = 0; i < 30; i++) {
        if (!simulateSingleTick(newstate)) {
            break;
        }
    }
    for (Point target : nonTargets) {
        if (GET(target.x, target.y, state).nodeType < WATER && GET(target.x, target.y, newstate).nodeType >= WATER) {
//            print(state);
//            print(newstate);
            return false;
        }
    }

    return true;
}

void generateNew(Action* action, Node nodes[HEIGHT][WIDTH], int flips[], std::vector<Point>& targets) {

    bool wasWater[targets.size()];
    for (int i = 0; i < targets.size(); i++) {
        Point target = targets[i];
        wasWater[i] = GET(target.x, target.y, nodes).nodeType >= WATER;
    }

    for (Point flip : action->flips) {
        Node node = GET(flip.x, flip.y, nodes);

        if (node.nodeType == BLOCK) set(nodes, flip.x, flip.y, AIR);
        else set(nodes, flip.x, flip.y, BLOCK);
    }
//            if (!flips.isEmpty() && !foundWater) return null;
    for (int i = 0; i < action->moves; i ++)
        simulateSingleTick(nodes);


    for (int i = 0; i < targets.size(); i++) {
        Point target = targets[i];
        if (!wasWater[i] && GET(target.x, target.y, nodes).nodeType >= WATER) {
            flips[i]++;
        }
    }
}

std::vector<Action *> anneal(Node beginState[HEIGHT][WIDTH],
                             std::vector<Point>& targets,
                             std::vector<Point>& nonTargets,
                             std::vector<Action*>& currentActions,
                             std::vector<int>& idxes,
                             double tempMult,
                             double targetTemp,
                             int targetIteration) {


    std::uniform_real_distribution<double> unifDistribution(0,1);
    std::default_random_engine re;


    std::vector<Point> combinedTargets;
    combinedTargets.reserve(targets.size() + nonTargets.size());
    for (Point target : targets) {
        combinedTargets.push_back(target);
    }
    for (Point target : nonTargets) {
        combinedTargets.push_back(target);
    }

//    Action doNothing = Action {"nothing", std::vector<Point>(0), 1 };


    double lastEvaluation = INFINITY;

    double temperature = 2.3;
    int iteration = 0;
    int lastTarget = 0;

    int lastUpdate = 0;

    double currentMinimum = 99999999;
    std::vector<Action*> currentMinimumActions;
    currentMinimumActions.reserve(currentActions.size());


    std::uniform_int_distribution<int> actionDist(0,currentActions.size());
    std::uniform_int_distribution<int> idxDist(0,idxes.size());
    while(true) {
        auto start = std::chrono::high_resolution_clock::now();

        int swapX = actionDist(re) % currentActions.size();
        int swapY = idxDist(re) % idxes.size();

        if (std::find(idxes.begin(), idxes.end(), swapX) != idxes.end()) continue;

        int realSwapY = idxes[swapY];



        // do swap
        Action* actionAtX = currentActions[swapX];
        Action* actionAtY = currentActions[realSwapY];

//        std::cout << "Swap between "<< realSwapY << " ? " << swapY << "?" << swapX << actionAtX->name << "and " << actionAtY->name << std::endl;

        currentActions[swapX] = actionAtY;
        currentActions[realSwapY] = actionAtX;

        // did swap.

        // evaluate solution.
        Node begin[HEIGHT][WIDTH];
        clone(beginState, begin);
        int flips[combinedTargets.size()];
        for (int i = 0; i < combinedTargets.size(); i++)
            flips[i] = 0;

        int chainLength = 0;
        int untilAction = 0;
        bool done = false;
        for (Action* currentAction : currentActions) {
            chainLength += currentAction->moves;
            untilAction += 1;
            generateNew(currentAction, begin, flips, combinedTargets);


            if (isDone(begin, flips, combinedTargets.size(), targets.size(), nonTargets)) {
                done = true;
                break;
            }
        }

        double additionalPenalty = 0;
        if (done) {
            int flipIdxes = 0;
            for (int i = 0; i < untilAction; i++) {
                if (!currentActions[i]->flips.empty()) {
                    flipIdxes += i;
                }
            }
            additionalPenalty += flipIdxes/10.0;
        } else {
            additionalPenalty += 10000;

            for (int i = 0; i < combinedTargets.size(); i++) {
                if ((flips[i] % 2 == 1) != (i < targets.size())) {
                    additionalPenalty += 20;
                }
            }

        }

        double currentEvaluation = additionalPenalty + chainLength;


        double variation = currentEvaluation - lastEvaluation;
        bool accepted = false;
        if (variation < 0) {
            accepted = true;
            lastTarget = untilAction;
        } else {
            if (unifDistribution(re) < exp(-variation / temperature)) {
                accepted = true;
            }
        }

        if (accepted) {
            idxes[swapY] = swapX;

            lastEvaluation = currentEvaluation;
        } else {
            currentActions[swapX] = actionAtX;
            currentActions[realSwapY] = actionAtY;
        }

        iteration++;
        if (iteration % 1000 == 0) {
            std::cout << iteration << "/" << lastEvaluation << "/" << temperature << "/" << chainLength << "/" << currentMinimum << "/" << lastTarget << std::endl;
            auto stop = std::chrono::high_resolution_clock::now();
            auto duration = duration_cast<std::chrono::nanoseconds>(stop - start);
            std::cout << "This Iteration took" <<  duration.count() << "nanoseconds" << std::endl;
        }
        temperature *= tempMult;

        if (lastEvaluation > 10000 && temperature < 2.0) {
            temperature = 2.3; // :D
        }

        if (currentEvaluation <= currentMinimum) {
            lastUpdate = iteration;
            currentMinimum = currentEvaluation;

            currentMinimumActions.clear();
            for (int i = 0; i < untilAction; i++) {
                currentMinimumActions.push_back(currentActions[i]);
            }
        }

        if (temperature < targetTemp) {
            return currentMinimumActions;
        }
        if (iteration - lastUpdate > targetIteration && lastEvaluation < 10000) {
            return currentMinimumActions;
        }
    }
}