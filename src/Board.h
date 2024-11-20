#ifndef BOARD_H
#define BOARD_H

#include "Constants.h"
#include "Ship.h"
#include <vector>

// Represents the game board
class Board {
private:
    CellState grid[BOARD_SIZE][BOARD_SIZE]; // The game grid
    std::vector<Ship> ships;                // List of ships on the board

public:
    // Constructor initializes the board
    Board();

    // Places a ship on the board
    bool placeShip(Ship& ship, int x, int y, Orientation orientation);

    // Checks if the ship placement is valid
    bool isValidPlacement(Ship& ship, int x, int y, Orientation orientation) const;

    // Processes an attack on the board
    bool receiveAttack(int x, int y, Ship*& hitShip);

    // Displays two boards side by side
    void display(const Board& otherBoard, bool showPlayerShips, bool showOpponentShips) const;

    // Displays game score board
    void displayScoreboard(int playerMoves, int playerDestroyedShips, int aiMoves, int aiDestroyedShips) const;


    // Checks if all ships are sunk
    bool allShipsSunk() const;

    // Checks if a coordinate has already been attacked
    bool isAlreadyAttacked(int x, int y) const;
};

#endif // BOARD_H
