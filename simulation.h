//
// Created by syeyoung on 8/9/24.
//

#ifndef WATERBOARD_SIMULATION_H
#define WATERBOARD_SIMULATION_H

#include <cstdint>

enum NodeType {
    BLOCK, AIR, WATER, SOURCE
};

#define WIDTH 19
#define HEIGHT 25

struct Node {
    NodeType nodeType;
    uint8_t waterLevel;
    bool update;
};

#define GET(x,y,n) n[y][x]
#define UP(x,y, n) n[y-1][x]
#define LEFT(x,y, n) n[y][x-1]
#define RIGHT(x,y, n) n[y][x+1]
#define DOWN(x,y, n) n[y+1][x]
#define BOTTOM_LEFT(x,y,n) n[y+1][x-1]
#define BOTTOM_RIGHT(x,y,n) n[y+1][x+1]


void set(Node nodes[HEIGHT][WIDTH], uint8_t x, uint8_t y, NodeType nodeType);
bool simulateSingleTick(Node nodes[HEIGHT][WIDTH]);
void clone(Node nodes[HEIGHT][WIDTH], Node newNodes[HEIGHT][WIDTH]);
void print(Node nodes[HEIGHT][WIDTH]);

#endif //WATERBOARD_SIMULATION_H
