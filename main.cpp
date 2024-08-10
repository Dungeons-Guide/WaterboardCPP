#include <iostream>

#include "annealing.h"
#include "simulation.h"
int main() {

    // yay
    auto config =
            "X      XWXWX      X\n"
            "XX      X XX      X\n"
            "X     XXX X X      \n"
            "XXXXXXXXX XXXXXXXXX\n"
            "        X      XXXX\n"
            " X XXXXXXXXXXX XXXX\n"
            " X X          XXXXX\n"
            " XXXXXX XXXXXX XXXX\n"
            " X      XX    X    \n"
            "XX XXXXX X XX XXXX \n"
            "XX XXXXX X XX XXXX \n"
            "     X X   XX  XXX \n"
            " XXXXXXXXXX XX XXX \n"
            "   X XXXXXX        \n"
            "XX X XXXXXXXXXXX X \n"
            " X X XXXXXXXXXXX X \n"
            " X X    X        X \n"
            "XX XXXXXX XXXXXXXX \n"
            "        X X    XXX \n"
            " X XXX XXXXXXX     \n"
            " X   X  X XXXX XXXX\n"
            " XXX XXXX XXXX XXX \n"
            " XXX XXXX XXXX XXX \n"
            "                   \n"
            " XXX XXXX XXXX XXX ";
    
    uint8_t targets[] = {0, 4, 9, 14, 18};
    bool targetActivate[] = {false, true, false, true, true};

    Node nodes[HEIGHT][WIDTH];

    std::vector<Action*> actions;

    Action g = {"mainStream", {Point { 9,0 }}, 6};
    Action a = {"173:0", { Point {1, 18}, Point {4, 5}, Point {9, 11}, Point {14, 20}, Point {18, 14} }, 6};
    Action b = {"41:0", { Point {2, 7}, Point {7, 11}, Point {8, 4}, Point {8, 16}, Point {10, 18}, Point {14, 8}}, 6};
    Action c = {"155:0", { Point {4, 12}, Point {10, 4}, Point {13, 13}, Point {15, 19}} , 6};
    Action d = {"57:0", { Point {2, 9}, Point {3, 18}, Point {8, 18}, Point {12, 8}, Point {15, 13}} , 6};
    Action e = {"133:0", {Point {1, 8}, Point {2, 19}, Point {5, 11}, Point {8, 8}, Point {8, 20}, Point {11, 11}, Point {14, 6}, Point {18, 20}}, 6};
    Action f = {"172:0", {Point {3, 8}, Point {3, 11}, Point {9, 19}, Point {10, 16}, Point {13, 9}}, 6};

    Action nullAction = {"nothing", {}, 1};

    actions.push_back(&a);
    actions.push_back(&b);
    actions.push_back(&c);
    actions.push_back(&d);
    actions.push_back(&e);
    actions.push_back(&f);
    actions.push_back(&g);

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            NodeType nodeType = AIR;
            char val = config[y* (WIDTH+1) + x];
            if (val == ' ') {
                nodeType = AIR;
            } else if (val == 'W') {
                nodeType = SOURCE;
            } else if (val == 'X') {
                nodeType = BLOCK;
            }
            nodes[y][x].waterLevel = 0;
            nodes[y][x].nodeType = nodeType;
            nodes[y][x].update = false;
        }
    }

    std::vector<Point> ptTargets;
    std::vector<Point> notTargets;
//    for (int i = 0; i < 5; i++) {
//        if (targetActivate[i])
//            ptTargets.push_back(Point{targets[i], HEIGHT - 1});
//        else
//            notTargets.push_back(Point{targets[i], HEIGHT - 1});
//    }
    ptTargets.push_back(Point{18, 24});
    ptTargets.push_back(Point{14, 24});
    ptTargets.push_back(Point{4, 24});;
    notTargets.push_back(Point{0, 24});;
    notTargets.push_back(Point{9, 24});

    std::vector<Action*> currentActions;
    for (int i = 0; i < 30; i++) {
        currentActions.push_back(&nullAction);
    }

    std::vector<int> idxes;
    size_t idx = currentActions.size();
    for (Action* availableAction : actions) {
        for (int j = 0; j < 3; j ++) {
            currentActions.push_back(availableAction); // add 15 actions.
            idxes.push_back(idx++);
        }
    }

    std::vector<Action*> solution = anneal(nodes, ptTargets, notTargets, currentActions, idxes, 0.9999, 0.1, 10000);

    std::vector<Point> combinedTargets;
    combinedTargets.reserve(ptTargets.size() + notTargets.size());
    for (Point target : ptTargets) {
        combinedTargets.push_back(target);
    }
    for (Point target : notTargets) {
        combinedTargets.push_back(target);
    }


    int flips[5] = {0};
    int cost = 0;
    for (Action* currentAction : solution) {
        cost += currentAction->moves;
        std::cout << currentAction->name << std::endl;
        generateNew(currentAction, nodes, flips, combinedTargets);
    }

    print(nodes);

    for (int i = 0; i < 5; i++) {
        std::cout << flips[i] << " ";
    }
    std::cout << std::endl;

    std::cout << cost * 5 / 20.0 << "s" << std::endl;


//    System.out.println(System.currentTimeMillis() - start);

    return 0;
}
