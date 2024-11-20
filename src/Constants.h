#ifndef CONSTANTS_H
#define CONSTANTS_H

// Board dimensions
const int BOARD_SIZE = 10;

// Represents the state of each cell on the board
enum CellState {
    EMPTY,   // No ship or attack
    SHIP,    // Ship is present
    HIT,     // Ship has been hit
    MISS     // Attack missed
};

// Represents the orientation of the ship
enum Orientation {
    HORIZONTAL,
    VERTICAL
};

#endif // CONSTANTS_H
