#include "Board.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <utility>

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
    // Check boundaries first
    if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) {
        std::cerr << "Attack coordinates out of bounds: (" << x << ", " << y << ")\n";
        hitShip = nullptr;
        return false;
    }

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


// Displays two boards side by side: player's board and opponent's board
void Board::display(const Board& opponentBoard, bool showPlayerShips, bool showOpponentShips) const {
    std::cout << "Your Board:                                    AI's Board:\n";

    // Print column headers for both boards
    std::cout << "   ";
    for (int i = 0; i < BOARD_SIZE; i++)
        std::cout << static_cast<char>('A' + i) << "  ";
    std::cout << "                ";
    for (int i = 0; i < BOARD_SIZE; i++)
        std::cout << static_cast<char>('A' + i) << "  ";
    std::cout << std::endl;

    // Print rows of both boards side by side
    for (int i = 0; i < BOARD_SIZE; i++) {
        // Row number for the player's board
        std::cout << std::setw(2) << i + 1 << " ";

        // Player's board row
        for (int j = 0; j < BOARD_SIZE; j++) {
            char playerSymbol = '.'; // Default: empty cell
            if (grid[i][j] == MISS)
                playerSymbol = 'o'; // Missed attack
            else if (grid[i][j] == HIT)
                playerSymbol = 'x'; // Successful hit
            else if (showPlayerShips && grid[i][j] == SHIP)
                playerSymbol = 'S'; // Show ship if allowed
            std::cout << playerSymbol << "  ";
        }

        // Separator between boards
        std::cout << "             ";

        // Row number for the opponent's board
        std::cout << std::setw(2) << i + 1 << " ";

        // Opponent's board row
        for (int j = 0; j < BOARD_SIZE; j++) {
            char opponentSymbol = '.'; // Default: empty cell
            if (opponentBoard.grid[i][j] == MISS)
                opponentSymbol = 'o'; // Missed attack
            else if (opponentBoard.grid[i][j] == HIT)
                opponentSymbol = 'x'; // Successful hit
            else if (showOpponentShips && opponentBoard.grid[i][j] == SHIP)
                opponentSymbol = 'S'; // Show ship if allowed
            std::cout << opponentSymbol << "  ";
        }

        std::cout << std::endl;
    }
}

// Displays the scoreboard during the game
void Board::displayScoreboard(int playerMoves, int playerDestroyedShips, int aiMoves, int aiDestroyedShips) const {
    std::cout << "\n=== Scoreboard ===\n";
    std::cout << "Player Moves: " << playerMoves << "   Ships Destroyed: " << playerDestroyedShips << "\n";
    std::cout << "AI Moves:     " << aiMoves << "   Ships Destroyed: " << aiDestroyedShips << "\n";
    std::cout << "==================\n\n";
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
