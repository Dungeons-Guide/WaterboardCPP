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
            "XX X X XXXXXX X XXX\n"
            "   X X        X    \n"
            " XX  XXXXX XXXXXXX \n"
            " XX XXXXXX     XXX \n"
            "           XXX XXX \n"
            " XXXXXX XX XXX     \n"
            "      X XX XXXXXXX \n"
            "XXX X X XX         \n"
            "    X X  XXXXXXXX X\n"
            " XXXX XX X      X X\n"
            "      XX X XXXXXX X\n"
            "XXXXX XX X X      X\n"
            "XX X     X X XXXXX \n"
            "XX X XXX X X XXXXX \n"
            "XX X XXX   X       \n"
            "   X XXXX XXXX XXXX\n"
            " XXX XXXX XXXX XXX \n"
            " XXX XXXX XXXX XXX \n"
            "                   \n"
            " XXX XXXX XXXX XXX \n";
    
    uint8_t targets[] = {0, 4, 9, 14, 18};
    bool targetActivate[] = {false, true, true, true,false};

    Node nodes[HEIGHT][WIDTH];

    std::vector<Action*> actions;

    Action g = {"mainStream", {Point { 9,0 }}, 3};
    Action a = {"57:0", { Point { 2, 9 }, Point { 5, 16 }, Point { 10, 4 }, Point { 14, 20 }, Point { 15, 13 }}, 3};
    Action b = {"41:0", { Point { 3, 12 }, Point { 4, 5 }, Point { 13, 5 }, Point { 18, 16 }}, 3};
    Action c = {"155:0", { Point { 2, 16 }, Point { 6, 5 }, Point { 8, 4 }} , 3};
    Action d = {"135:0", { Point { 2, 5 }, Point { 8, 18 }, Point { 10, 10 }, Point { 15, 4 }, Point { 16, 16 }, Point { 18, 20 }} , 3};
//    Action e = {"5", {}, 3};
//    Action f = {"6", {}, 3};
    Action nullAction = {"nothing", {}, 1};

    actions.push_back(&g);
    actions.push_back(&a);
    actions.push_back(&b);
    actions.push_back(&c);
    actions.push_back(&d);

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
    for (int i = 0; i < 5; i++) {
        if (targetActivate[i])
            ptTargets.push_back(Point{targets[i], HEIGHT - 1});
        else
            notTargets.push_back(Point{targets[i], HEIGHT - 1});
    }

    std::vector<Action*> solution = anneal(nodes, ptTargets, notTargets, actions, &nullAction);

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
