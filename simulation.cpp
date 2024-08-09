//
// Created by syeyoung on 8/9/24.
//
#include <iostream>
#include "simulation.h"



void clone(Node  nodes[HEIGHT][WIDTH], Node newNodes[HEIGHT][WIDTH]) {

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            newNodes[y][x].waterLevel = nodes[y][x].waterLevel;
            newNodes[y][x].nodeType = nodes[y][x].nodeType;
            newNodes[y][x].update = nodes[y][x].update;
        }
    }
}


inline int max(int a, int b) {
    return a < b ? b : a;
}

void print(Node nodes[HEIGHT][WIDTH]) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            Node n = nodes[y][x];
            if (n.nodeType == AIR)
                std::cout << " ";
            else if (n.nodeType == BLOCK)
                std::cout << "X";
            else if (n.nodeType == SOURCE)
                std::cout << "W";
            else
                std::cout << unsigned(n.waterLevel);
        }
        std::cout << std::endl;
    }
}

void set(Node nodes[HEIGHT][WIDTH], uint8_t x, uint8_t y, NodeType nodeType) {
    GET(x,y, nodes).nodeType = nodeType;
    GET(x,y, nodes).waterLevel = 0;
    GET(x,y, nodes).update = true;
}

int8_t getFlowDirection(uint8_t x, uint8_t y, Node nodes[HEIGHT][WIDTH]) {
    int right = 0;
    uint8_t rightPt_x = x+1;
    uint8_t rightPt_y = y;
    while (true) {
        right++;
        if (right == 8) {
            right = 999; break;
        }
        if (rightPt_x == WIDTH) {
            right = 999; break;
        }
        if (GET(rightPt_x, rightPt_y, nodes).nodeType == BLOCK) {
            right = 999; break;
        }
        if (DOWN(rightPt_x, rightPt_y, nodes).nodeType != BLOCK) {
            break;
        }

        rightPt_x++;
    }
    int left = 0;

    uint8_t leftPt_x = x-1;
    uint8_t leftPt_y = y;
    while (true) {
        left++;
        if (left == 8) {
            left = 999; break;
        }
        if (GET(leftPt_x, leftPt_y, nodes).nodeType == BLOCK) {
            left = 999; break;
        }
        if (DOWN(leftPt_x, leftPt_y, nodes).nodeType != BLOCK) {
            break;
        }


        if (leftPt_x == 0) { // :D we don't go into negatives.
            left = 999; break;
        }
        leftPt_x--;
    }

    if (left == right) return 0;
    if (left > right) return 1;
    return -1;
}

bool shouldUpdate(uint8_t x, uint8_t y, Node nodes[HEIGHT][WIDTH]) {
    return GET(x,y,nodes).update || (x < WIDTH -1 && RIGHT(x,y,nodes).update)
           || (x > 0 && LEFT(x,y,nodes).update)
           || (y > 0 && UP(x,y,nodes).update)
           || (y < HEIGHT - 1 && DOWN(x,y,nodes).update);
}

bool doTick(Node nodes[HEIGHT][WIDTH], Node nodesNew[HEIGHT][WIDTH], uint8_t x, uint8_t y) {
    Node prev = GET(x,y,nodes);
    uint8_t maxWaterLv = max(0, prev.nodeType == SOURCE ? 8 : prev.waterLevel - 1);

    if (prev.nodeType == AIR || prev.nodeType == WATER) {
        if (y > 0 && UP(x,y,nodes).nodeType >= WATER) { // water or source
            maxWaterLv = 8;
        }
        if (x > 0 && LEFT(x,y,nodes).nodeType >= WATER) {
            bool isSource = LEFT(x,y,nodes).nodeType == SOURCE;
            NodeType bottomLeft = y == HEIGHT - 1 ? BLOCK : BOTTOM_LEFT(x,y,nodes).nodeType;
            if (prev.nodeType == WATER || (
                    shouldUpdate(x-1,y,nodes) && (bottomLeft == BLOCK || (isSource && bottomLeft != AIR)) && getFlowDirection(x-1,y,nodes) >= 0)) {
                maxWaterLv = max(maxWaterLv, LEFT(x,y,nodes).waterLevel - 1);
            }
        }
        if (x < WIDTH-1 && RIGHT(x,y,nodes).nodeType >= WATER) {
            bool isSource = RIGHT(x,y,nodes).nodeType == SOURCE;
            NodeType bottomRight = y == HEIGHT - 1 ? BLOCK : BOTTOM_RIGHT(x,y,nodes).nodeType;
            if (prev.nodeType == WATER || (
                    shouldUpdate(x+1,y,nodes) && (bottomRight == BLOCK || (isSource && bottomRight != AIR)) && getFlowDirection(x+1,y,nodes) <= 0)) {
                maxWaterLv = max(maxWaterLv, RIGHT(x,y,nodes).waterLevel - 1);
            }
        }
    }
    GET(x,y,nodesNew).nodeType = prev.nodeType;
    GET(x,y,nodesNew).waterLevel = maxWaterLv;
    GET(x,y,nodesNew).update = false;
    if (maxWaterLv == 0 && nodesNew[y][x].nodeType == WATER)
        GET(x,y,nodesNew).nodeType = AIR;
    else if (maxWaterLv > 0 && nodesNew[y][x].nodeType == AIR)
        GET(x,y,nodesNew).nodeType = WATER;

    if (prev.nodeType != GET(x,y,nodesNew).nodeType || prev.waterLevel != GET(x,y,nodesNew).waterLevel) {
        GET(x,y,nodesNew).update = true;
    }
    return GET(x,y,nodesNew).update;
}

bool simulateSingleTick(Node nodes[HEIGHT][WIDTH]) {
    Node nodesNew[HEIGHT][WIDTH];
    bool update = false;

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (doTick(nodes, nodesNew, x,y)) update =true;

            if ( GET(x,y,nodesNew).waterLevel - GET(x,y,nodes).waterLevel > 0 && GET(x,y,nodes).nodeType == WATER && GET(x,y,nodesNew).nodeType == WATER) {
                Node prev = GET(x,y,nodes);

                GET(x,y,nodes) = GET(x,y, nodesNew);
                if (x > 0)
                    doTick(nodes, nodesNew, x-1,y);
                if (x < WIDTH - 1)
                    doTick(nodes, nodesNew, x+1,y);

                GET(x,y,nodes) = prev;
            }
        }
    }

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            GET(x,y,nodes).nodeType = GET(x,y,nodesNew).nodeType;
            GET(x,y,nodes).waterLevel = GET(x,y,nodesNew).waterLevel;
            GET(x,y,nodes).update = GET(x,y,nodesNew).update;
        }
    }
    return update;
}
