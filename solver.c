#include "solver.h"
#include "mouse.h"
#include <stdio.h>

#define QUEUE_SIZE 50
#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3
#define FALSE 0
#define TRUE 1

// This function redirects function calls from mouse.c to the desired maze solving algorithm
Action solver(Mouse *mouse)
{
    // This can be changed to call other maze solving algorithms
    return floodFill(mouse);
}

// Simple algorithm; mouse goes straight until encountering a wall, then preferentially turns left
Action obstacleAvoider(Mouse *mouse)
{
    if(getFrontReading(mouse) == 0) return FORWARD;
    else if(getLeftReading(mouse) == 0) return LEFT;
    else if(getRightReading(mouse) == 0) return RIGHT;
    else return LEFT;
}

// Left wall following algorithm
int turnedLeft = 0;
Action leftWallFollower(Mouse *mouse)
{
    if(turnedLeft)
    {
        turnedLeft = 0;
        return FORWARD;
    }
    else if(getLeftReading(mouse) == 0)
    {   
        turnedLeft = 1;
        return LEFT;
    }
    else if(getFrontReading(mouse) == 0) return FORWARD;
    else return RIGHT;
}

// Floodfill algorithm
int mDist[6][6] = {
    {5,4,3,2,1,0},
    {6,5,4,3,2,1},
    {7,6,5,4,3,2},
    {8,7,6,5,4,3},
    {9,8,7,6,5,4},
    {10,9,8,7,6,5},
};

int hWalls[7][6] = {
    {1,1,1,1,1,1},
    {0,0,0,0,0,0},
    {0,0,0,0,0,0},
    {0,0,0,0,0,0},
    {0,0,0,0,0,0},
    {0,0,0,0,0,0},
    {1,1,1,1,1,1},
};

int vWalls[6][7] = {
    {1,0,0,0,0,0,1},
    {1,0,0,0,0,0,1},
    {1,0,0,0,0,0,1},
    {1,0,0,0,0,0,1},
    {1,0,0,0,0,0,1},
    {1,0,0,0,0,0,1},
};

int initialized = FALSE;
struct position pos;
int dir;
Action floodFill(Mouse *mouse) { 

    // start at (5,0), end at (0,5)

    // init
    if (initialized == FALSE){    
        pos.x = 5;
        pos.y = 0; 
        initialized = TRUE;
        dir = NORTH;
    }

    int rightReading = getRightReading(mouse);
    int frontReading = getFrontReading(mouse);
    int leftReading = getLeftReading(mouse);
    int currMD = mDist[pos.x][pos.y];

    // end of maze
    if (mDist[pos.x][pos.y] == 0)
        return IDLE;
    
    // ================== updateWalls ================= //
    updateWalls(mouse);

    // ================== moveMouse ================= //
    
    // sort by direction
    switch(dir){
        case NORTH: 
            if ((frontReading == 0) && (currMD > mDist[pos.x-1][pos.y])){       //if front is open and has a smaller dist
                //update current coordinates
                pos.x--;
                return FORWARD;
            }

            else if ((leftReading == 0) && (currMD > mDist[pos.x][pos.y-1])){     //if left is open and has a smaller dist
                dir = WEST;
                return LEFT;
            }
            
            else if ((rightReading == 0) && (currMD > mDist[pos.x][pos.y+1])){    //if right is open and has a smaller dist
                dir=EAST;
                return RIGHT;
            }
            else {
                dir=WEST;
                return LEFT;
            }
            break;
        
        case EAST: 
            if ((frontReading == 0) && (currMD > mDist[pos.x][pos.y+1])){
                pos.y++;
                return FORWARD;
            }
            else if ((leftReading == 0) && (currMD > mDist[pos.x-1][pos.y])){
                dir=NORTH;
                return LEFT;
            }
            
            else if((rightReading == 0) && (currMD > mDist[pos.x+1][pos.y])){
                dir=SOUTH;
                return RIGHT;
            }
            else if ((rightReading != 0) && (leftReading != 0) && (frontReading != 0)){
                dir=NORTH;
                return LEFT;
            }
            break;
        
        case SOUTH: 
            if ((frontReading == 0) && (currMD > mDist[pos.x+1][pos.y])){
                pos.x++;
                return FORWARD;
            }
            else if ((leftReading == 0) && (currMD > mDist[pos.x][pos.y+1])){
                dir=EAST;
                return LEFT;
            }
            else if((rightReading == 0) && (currMD > mDist[pos.x][pos.y-1])){
                dir=WEST;
                return RIGHT;
            }
            else if ((rightReading != 0) && (leftReading != 0) && (frontReading != 0)){
                dir=EAST;
                return LEFT;
            }
            break;
        
        case WEST: 
            if ((frontReading == 0) && (currMD > mDist[pos.x][pos.y-1])){
                pos.y--;
                return FORWARD;
            }
            else if ((leftReading == 0) && (currMD > mDist[pos.x+1][pos.y])){
                dir=SOUTH;
                return LEFT;
            }
            else if((rightReading == 0) && (currMD > mDist[pos.x-1][pos.y])){
                dir=NORTH;
                return RIGHT;
            }
            else if ((rightReading != 0) && (leftReading != 0) && (frontReading != 0)){
                dir = SOUTH;
                return LEFT;
            }
            break;
    }

    // if stuck, perform floodfill
    struct position currPos = pos;
    doFloodFill(currPos);

    // ==================moveMouse================== //

    // perform movement logic with recalculated manhattan distances
    switch(dir){
        case NORTH: 
            if ((frontReading == 0) && (currMD > mDist[pos.x-1][pos.y])){       //if front is open and has a smaller dist
                //update current coordinates
                pos.x--;
                return FORWARD;
            }
            else if ( (leftReading == 0) && (currMD > mDist[pos.x][pos.y-1])){     //if left is open and has a smaller dist
                dir = WEST;
                return LEFT;
            }
            else if ((rightReading == 0) && (currMD > mDist[pos.x][pos.y+1])){    //if right is open and has a smaller dist
                dir=EAST;
                return RIGHT;
            }
            else {
                dir=WEST;
                return LEFT;
            }
            break;
            
        case EAST: 
            if ( (frontReading == 0) && (currMD > mDist[pos.x][pos.y+1]) ){
                pos.y++;
                return FORWARD;
            }
            else if ( (leftReading == 0) && (currMD > mDist[pos.x-1][pos.y]) ){
                dir=NORTH;
                return LEFT;
            }
            
            else if( (rightReading == 0) && (currMD > mDist[pos.x+1][pos.y]) ){
                dir=SOUTH;
                return RIGHT;
            }
            else if ((rightReading != 0) && (leftReading != 0) && (frontReading != 0)){
                dir=NORTH;
                return LEFT;
            }
            break;
        
        case SOUTH: 
            if ( (frontReading == 0) && (currMD > mDist[pos.x+1][pos.y]) ){
                pos.x++;
                return FORWARD;
            }
            else if ( (leftReading == 0) && (currMD > mDist[pos.x][pos.y+1]) ){
                dir=EAST;
                return LEFT;
            }
            
            else if( (rightReading == 0) && (currMD > mDist[pos.x][pos.y-1]) ){
                dir=WEST;
                return RIGHT;
            }
            else if ((rightReading != 0) && (leftReading != 0) && (frontReading != 0)){
                dir=EAST;
                return LEFT;
            }
            break;

        case WEST: 
            if ( (frontReading == 0) && (currMD > mDist[pos.x][pos.y-1]) ){
                pos.y--;
                return FORWARD;
            }
            else if ( (leftReading == 0) && (currMD > mDist[pos.x+1][pos.y]) ){
                dir=SOUTH;
                return LEFT;
            }
            else if( (rightReading == 0) && (currMD > mDist[pos.x-1][pos.y]) ){
                dir=NORTH;
                return RIGHT;
            }
            else if ((rightReading != 0) && (leftReading != 0) && (frontReading != 0)){
                dir = SOUTH;
                return LEFT;
            }
            break;
    }
}

// ==================updateWalls================== //
void updateWalls(Mouse *mouse) {   
    switch(dir){
        case NORTH:
            if(getFrontReading(mouse) != 0)
                hWalls[pos.x][pos.y]=1;
            if(getLeftReading(mouse) != 0)
                vWalls[pos.x][pos.y]=1;
            if(getRightReading(mouse) != 0)
                vWalls[pos.x][pos.y+1]=1;
            break;

        case EAST:
            if(getFrontReading(mouse) != 0)
                vWalls[pos.x][pos.y+1]=1;
            if(getLeftReading(mouse) != 0)
                hWalls[pos.x][pos.y]=1;
            if(getRightReading(mouse) != 0)
                hWalls[pos.x+1][pos.y]=1;
            break;

        case SOUTH:
            if(getFrontReading(mouse) != 0)
                hWalls[pos.x+1][pos.y]=1;
            if(getLeftReading(mouse) != 0)
                vWalls[pos.x][pos.y+1]=1;
            if(getRightReading(mouse) != 0)
                vWalls[pos.x][pos.y]=1;
            break;
    
        case WEST:
            if(getFrontReading(mouse) != 0)
                vWalls[pos.x][pos.y]=1;
            if(getLeftReading(mouse) != 0)
                hWalls[pos.x+1][pos.y]=1;
            if(getRightReading(mouse) != 0)
                hWalls[pos.x][pos.y]=1;
            break;
    }
}
// ==================floodfill================== //
void doFloodFill(struct position currPos) {

    // printf("i'm gonna flooodfiiiilll\n");
    // for(int i = 0; i < 6; i++){ // 7 for hWalls
    //     for(int j = 0; j < 6; j++) // 7 for vWalls
    //         printf("%d ", mDist[i][j]);
        
    //     printf("\n");
    // }

    // enqueue current cell
    enqueue(currPos);

    // while queue isn't empty
    while(!empty()){
        
        // Take front cell in queue “out of line” for consideration
        struct position curr = dequeue();
        
        // keep track of accessible neighbors
        int topValid = FALSE;
        int leftValid = FALSE;
        int rightValid = FALSE;
        int botValid = FALSE;
        int minDist = -1;

        // Get front cell’s minimum value amongst accessible neighbors using hWalls/vWalls

        // check if top is valid
        if (hWalls[curr.x][curr.y] == 0) {          
            topValid=TRUE;
            // printf("top valid true\n");
            // if minDist hasnt been set, set it to top
            if (minDist==-1 || mDist[curr.x-1][curr.y]<minDist)
                minDist = mDist[curr.x-1][curr.y];
        }

        // check if left is valid
        if (vWalls[curr.x][curr.y] == 0) {         
            leftValid=TRUE;
            // printf("left valid true\n");
            if (minDist==-1 || mDist[curr.x][curr.y-1]<minDist)
                minDist = mDist[curr.x][curr.y-1];
        }

        // check if bottom is valid
        if (hWalls[curr.x+1][curr.y] == 0) {     
            botValid=TRUE;
            // printf("bot valid true\n");
            if (minDist==-1 || mDist[curr.x+1][curr.y]<minDist)
                minDist = mDist[curr.x+1][curr.y];
            
        }
        
        // check if right is valid
        if (vWalls[curr.x][curr.y+1] == 0){      //check if right is valid
            rightValid=TRUE;
            // printf("right valid true\n");
            if (minDist==-1 || mDist[curr.x][curr.y+1]<minDist)
                minDist = mDist[curr.x][curr.y+1];
            
        }
        
        // If front cell’s value ≤ minimum of its neighbors
        if (mDist[curr.x][curr.y] <= minDist){
            
            // set front cell’s value to minimum + 1
            mDist[curr.x][curr.y] = minDist+1;
   
            // add all accessible neighbors to queue.
            if (topValid == TRUE){
                struct position topNeighbor;
                topNeighbor.x=curr.x-1;
                topNeighbor.y=curr.y;
                enqueue(topNeighbor);
            }

            if (botValid == TRUE){
                struct position bottomNeighbor;
                bottomNeighbor.x=curr.x+1;
                bottomNeighbor.y=curr.y;
                enqueue(bottomNeighbor);
            }

            if (leftValid == TRUE){
                struct position leftNeighbor;
                leftNeighbor.x=curr.x;
                leftNeighbor.y=curr.y-1;
                enqueue(leftNeighbor);
            }

            if (rightValid == TRUE){
                struct position rightNeighbor;
                rightNeighbor.x=curr.x;
                rightNeighbor.y=curr.y+1;
                enqueue(rightNeighbor);
            }
        }
    }
}

// queue helper functions
struct position circularQueue[QUEUE_SIZE];
int front = -1, rear = -1;

// checks if queue is empty
int empty() {
    if (front == -1) 
        return TRUE;
    return FALSE;
}

// enqueues current struct
void enqueue(struct position tempPos) {
    if (front == -1) 
        front = 0;
    rear = (rear + 1) % QUEUE_SIZE;
	circularQueue[rear] = tempPos;
}

// dequeues and returns current struct
struct position dequeue() {
    struct position tempPos = circularQueue[front];
    // queue has only one element
	if(front == rear) 
		front = rear = -1;
    else
        front = (front + 1) % QUEUE_SIZE;
    return tempPos;
}

// some pseudocode
/*
check if front cell open and mdist is less
    update all the stuff
    go forward
check if left cell is open and mdist is less
    update all the stuff
    go forward
check if right cell open and mdist is less
    update all the stuff
    go forward
else recalculate MD

floodfill:

enqueue current cell
while queue isnt empty
    dequeue
    check which neighbors are accessible (using getfront/right/leftreading)

    get the lowest value out of those numbers
    if front cell's value <=minimum of neighbors
        set front cell's value to minimum+1
        enqueue all neighbors
*/

// fin