// Board.cpp
#include "Board.h"
#include <iostream>
#include <iomanip>

// Constructor initializes the game grid
Board::Board() {
    for (int i = 0; i < BOARD_SIZE; i++)
        for (int j = 0; j < BOARD_SIZE; j++)
            grid[i][j] = EMPTY;
}

// Checks if the ship placement is valid
bool Board::isValidPlacement(Ship& ship, int x, int y, Orientation orientation) const {
    int dx = 0, dy = 0;
    if (orientation == HORIZONTAL)
        dx = 1;
    else
        dy = 1;

    for (int i = 0; i < ship.getSize(); i++) {
        int nx = x + dx * i;
        int ny = y + dy * i;

        // Check boundaries
        if (nx < 0 || nx >= BOARD_SIZE || ny < 0 || ny >= BOARD_SIZE)
            return false;

        // Check if the cell is already occupied
        if (grid[ny][nx] != EMPTY)
            return false;
    }
    return true;
}

// Places a ship on the board
bool Board::placeShip(Ship& ship, int x, int y, Orientation orientation) {
    if (!isValidPlacement(ship, x, y, orientation))
        return false;

    int dx = 0, dy = 0;
    if (orientation == HORIZONTAL)
        dx = 1;
    else
        dy = 1;

    // Place the ship on the grid and store its coordinates
    for (int i = 0; i < ship.getSize(); i++) {
        int nx = x + dx * i;
        int ny = y + dy * i;
        grid[ny][nx] = SHIP;
        ship.addCoordinate(nx, ny);
    }

    ships.push_back(ship);
    return true;
}

// Processes an attack on the board
bool Board::receiveAttack(int x, int y, Ship*& hitShip) {
    // Check if the coordinate has already been attacked
    if (grid[y][x] == MISS || grid[y][x] == HIT) {
        hitShip = nullptr;
        return false;
    }

    if (grid[y][x] == EMPTY) {
        grid[y][x] = MISS; // Mark as miss
        hitShip = nullptr;
        return false;
    } else if (grid[y][x] == SHIP) {
        grid[y][x] = HIT; // Mark as hit

        // Find the ship that was hit
        for (auto& ship : ships) {
            if (ship.occupiesCoordinate(x, y)) {
                ship.hit();
                hitShip = &ship;
                return true;
            }
        }
    }
    return false;
}

// Displays the board
void Board::display(bool showShips) const {
    std::cout << "  ";
    for (int i = 0; i < BOARD_SIZE; i++)
        std::cout << static_cast<char>('A' + i) << " ";
    std::cout << std::endl;

    for (int i = 0; i < BOARD_SIZE; i++) {
        std::cout << std::setw(2) << i + 1 << " ";
        for (int j = 0; j < BOARD_SIZE; j++) {
            char symbol = '.';
            if (grid[i][j] == MISS)
                symbol = 'o';
            else if (grid[i][j] == HIT)
                symbol = 'x';
            else if (showShips && grid[i][j] == SHIP)
                symbol = 'S';
            std::cout << symbol << " ";
        }
        std::cout << std::endl;
    }
}

// Checks if all ships are sunk
bool Board::allShipsSunk() const {
    for (const auto& ship : ships) {
        if (!ship.isSunk())
            return false;
    }
    return true;
}

// Checks if a coordinate has already been attacked
bool Board::isAlreadyAttacked(int x, int y) const {
    return grid[y][x] == MISS || grid[y][x] == HIT;
}
