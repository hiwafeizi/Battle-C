// main.cpp
#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <cctype>
#include <cstdlib>
#include <ctime>
#include "Board.h"
#include "Ship.h"
#include "Constants.h"

// Parses user input coordinates (e.g., "A5") into x and y indices
std::pair<int, int> parseCoordinate(const std::string& coord) {
    if (coord.length() < 2 || coord.length() > 3)
        return {-1, -1};

    char col = toupper(coord[0]);
    int row = std::stoi(coord.substr(1));

    int x = col - 'A';
    int y = row - 1;

    if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE)
        return {-1, -1};

    return {x, y};
}

int main() {
    // Initialize random seed
    srand(static_cast<unsigned>(time(0)));

    // Create player and AI boards
    Board playerBoard;
    Board aiBoard;

    // Define ships to place
    std::vector<std::pair<std::string, int>> shipInfo = {
        {"Carrier", 5},
        {"Battleship", 4},
        {"Cruiser", 3},
        {"Submarine", 3},
        {"Destroyer", 2}
    };

    // Player places ships
    std::cout << "=== Welcome to BattleC ===\n";
    std::cout << "Place your ships on the board:\n";

    for (const auto& info : shipInfo) {
        bool placed = false;
        while (!placed) {
            playerBoard.display(true);
            std::cout << "Place your " << info.first << " (Size: " << info.second << ")\n";
            std::cout << "Enter starting coordinate (e.g., A5): ";
            std::string coord;
            std::cin >> coord;

            std::pair<int, int> xy = parseCoordinate(coord);
            if (xy.first == -1) {
                std::cout << "Invalid coordinate. Try again.\n";
                continue;
            }

            std::cout << "Enter orientation (H for horizontal, V for vertical): ";
            char oriChar;
            std::cin >> oriChar;
            Orientation ori = (toupper(oriChar) == 'H') ? HORIZONTAL : VERTICAL;

            Ship ship(info.first, info.second);
            if (playerBoard.placeShip(ship, xy.first, xy.second, ori)) {
                placed = true;
            } else {
                std::cout << "Invalid placement. Try again.\n";
            }
        }
    }

    // AI places ships
    for (const auto& info : shipInfo) {
        bool placed = false;
        while (!placed) {
            int x = rand() % BOARD_SIZE;
            int y = rand() % BOARD_SIZE;
            Orientation ori = (rand() % 2 == 0) ? HORIZONTAL : VERTICAL;

            Ship ship(info.first, info.second);
            if (aiBoard.placeShip(ship, x, y, ori)) {
                placed = true;
            }
        }
    }

    // Initialize game variables
    bool gameOver = false;
    int playerMoves = 0;
    int aiMoves = 0;
    std::vector<std::pair<int, int>> aiGuesses;

    // AI targeting mode variables
    enum AITargetMode { HUNT, TARGET };
    AITargetMode aiMode = HUNT;
    std::vector<std::pair<int, int>> aiHits;
    std::vector<std::pair<int, int>> aiPotentialTargets;

    // Main game loop
    while (!gameOver) {
        // Player's turn
        bool playerTurn = true;
        while (playerTurn) {
            playerBoard.display(true);
            std::cout << "\nOpponent's Board:\n";
            aiBoard.display(false);

            std::cout << "Enter coordinate to attack (e.g., B6): ";
            std::string coord;
            std::cin >> coord;

            std::pair<int, int> xy = parseCoordinate(coord);
            if (xy.first == -1) {
                std::cout << "Invalid coordinate. Try again.\n";
                continue;
            }

            // Check if the coordinate has already been attacked
            if (aiBoard.isAlreadyAttacked(xy.first, xy.second)) {
                std::cout << "You've already attacked this coordinate. Try again.\n";
                continue;
            }

            Ship* hitShip = nullptr;
            bool hit = aiBoard.receiveAttack(xy.first, xy.second, hitShip);

            playerMoves++;

            if (hit) {
                std::cout << "It's a hit!\n";
                if (hitShip->isSunk()) {
                    std::cout << "You sank the opponent's " << hitShip->getName() << "!\n";
                }
                if (aiBoard.allShipsSunk()) {
                    std::cout << "Congratulations! You win!\n";
                    gameOver = true;
                    break;
                }
                // Player gets another turn on hit
            } else {
                std::cout << "You missed.\n";
                playerTurn = false;
            }
        }

        if (gameOver)
            break;

        // AI's turn
        bool aiTurn = true;
        while (aiTurn) {
            int x, y;

            if (aiMode == HUNT) {
                // Random selection
                do {
                    x = rand() % BOARD_SIZE;
                    y = rand() % BOARD_SIZE;
                } while (playerBoard.isAlreadyAttacked(x, y));
            } else if (aiMode == TARGET) {
                // Select from potential targets
                if (!aiPotentialTargets.empty()) {
                    auto target = aiPotentialTargets.back();
                    aiPotentialTargets.pop_back();
                    x = target.first;
                    y = target.second;
                } else {
                    aiMode = HUNT;
                    continue;
                }
            }

            Ship* hitShip = nullptr;
            bool hit = playerBoard.receiveAttack(x, y, hitShip);

            aiMoves++;

            std::cout << "\nAI attacks " << static_cast<char>('A' + x) << y + 1 << "\n";
            if (hit) {
                std::cout << "AI hits your ship!\n";

                // Add adjacent cells to potential targets
                if (aiMode == HUNT) {
                    aiMode = TARGET;
                    aiHits.push_back({x, y});

                    int dx[] = {-1, 1, 0, 0};
                    int dy[] = {0, 0, -1, 1};
                    for (int i = 0; i < 4; i++) {
                        int nx = x + dx[i];
                        int ny = y + dy[i];
                        if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE) {
                            if (!playerBoard.isAlreadyAttacked(nx, ny)) {
                                aiPotentialTargets.push_back({nx, ny});
                            }
                        }
                    }
                }

                if (hitShip->isSunk()) {
                    std::cout << "AI sank your " << hitShip->getName() << "!\n";
                    aiMode = HUNT;
                    aiPotentialTargets.clear();
                }

                if (playerBoard.allShipsSunk()) {
                    std::cout << "AI wins! You lose.\n";
                    gameOver = true;
                    break;
                }
                // AI gets another turn on hit
            } else {
                std::cout << "AI missed.\n";
                aiTurn = false;
            }
        }
    }

    // Display final statistics
    std::cout << "\n=== Game Over ===\n";
    std::cout << "Your moves: " << playerMoves << "\n";
    std::cout << "AI's moves: " << aiMoves << "\n";

    return 0;
}
