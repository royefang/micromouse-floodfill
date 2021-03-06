#ifndef SOLVER_H
#define SOLVER_H

#include "mouse.h"

Action solver(Mouse *mouse);
Action leftWallFollower(Mouse *mouse);
Action floodFill(Mouse *mouse);

struct position{
    int x;
    int y;
};

int empty();
void enqueue(struct position tempPos);
struct position dequeue();

#endif
