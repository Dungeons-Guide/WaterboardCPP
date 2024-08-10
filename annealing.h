//
// Created by syeyoung on 8/9/24.
//

#ifndef WATERBOARD_ANNEALING_H
#define WATERBOARD_ANNEALING_H

#include "simulation.h"
#include <map>
#include <string>
#include <vector>

struct Point {
    uint8_t x;
    uint8_t y;
};

struct Action {
    std::string name;
    std::vector<Point> flips;
    int moves;
};

void generateNew(Action* action, Node nodes[HEIGHT][WIDTH], int flips[], std::vector<Point>& targets);

std::vector<Action *> anneal(Node beginState[HEIGHT][WIDTH],
                             std::vector<Point>& targets,
                             std::vector<Point>& nonTargets,
                             std::vector<Action*>& currentActions,
                             std::vector<int>& idxes,
                             double tempMult,
                             double targetTemp,
                             int targetIteration);

#endif //WATERBOARD_ANNEALING_H
