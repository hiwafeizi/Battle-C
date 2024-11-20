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
    // Initialize game variables
    bool gameOver = false;
    int playerMoves = 0;
    int aiMoves = 0;
    int playerDestroyedShips = 0;
    int aiDestroyedShips = 0;

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
            // Show both boards
            playerBoard.display(aiBoard, true, false);

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
            // Show both boards and scores
            playerBoard.display(aiBoard, true, false);
            playerBoard.displayScoreboard(playerMoves, playerDestroyedShips, aiMoves, aiDestroyedShips);

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
                    playerDestroyedShips++;
                }
                if (aiBoard.allShipsSunk()) {
                    std::cout << "\nCongratulations! You won! You are a good fighter.\n";
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
                // Random selection using a checkerboard pattern
                do {
                    x = rand() % BOARD_SIZE;
                    y = rand() % BOARD_SIZE;
                } while ((x + y) % 2 != 0 || playerBoard.isAlreadyAttacked(x, y));
            } else if (aiMode == TARGET) {
                // Select from potential targets
                if (!aiPotentialTargets.empty()) {
                    auto target = aiPotentialTargets.front(); // FIFO: process oldest first
                    aiPotentialTargets.erase(aiPotentialTargets.begin());
                    x = target.first;
                    y = target.second;
                } else {
                    aiMode = HUNT; // No more targets, switch back to HUNT mode
                    continue;
                }
            }

            Ship* hitShip = nullptr;
            bool hit = playerBoard.receiveAttack(x, y, hitShip);

            aiMoves++;

            std::cout << "\nAI attacks " << static_cast<char>('A' + x) << y + 1 << "\n";
            if (hit) {
                std::cout << "AI hits your ship!\n";

                if (aiMode == HUNT) {
                    // Transition to TARGET mode after the first hit
                    aiMode = TARGET;
                    aiHits.push_back({x, y});
                } else if (aiHits.size() >= 2) {
                    // Determine ship orientation after two hits
                    auto firstHit = aiHits.front();
                    auto secondHit = aiHits.back();

                    if (firstHit.first == secondHit.first) {
                        // Horizontal ship
                        int row = firstHit.first;
                        int minCol = std::min(firstHit.second, secondHit.second);
                        int maxCol = std::max(firstHit.second, secondHit.second);

                        if (maxCol + 1 < BOARD_SIZE && !playerBoard.isAlreadyAttacked(row, maxCol + 1))
                            aiPotentialTargets.push_back({row, maxCol + 1});
                        if (minCol - 1 >= 0 && !playerBoard.isAlreadyAttacked(row, minCol - 1))
                            aiPotentialTargets.push_back({row, minCol - 1});
                    } else if (firstHit.second == secondHit.second) {
                        // Vertical ship
                        int col = firstHit.second;
                        int minRow = std::min(firstHit.first, secondHit.first);
                        int maxRow = std::max(firstHit.first, secondHit.first);

                        if (maxRow + 1 < BOARD_SIZE && !playerBoard.isAlreadyAttacked(maxRow + 1, col))
                            aiPotentialTargets.push_back({maxRow + 1, col});
                        if (minRow - 1 >= 0 && !playerBoard.isAlreadyAttacked(minRow - 1, col))
                            aiPotentialTargets.push_back({minRow - 1, col});
                    }
                }

                // Add adjacent cells to potential targets
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

                if (hitShip->isSunk()) {
                    std::cout << "AI sank your " << hitShip->getName() << "!\n";
                    aiDestroyedShips++;
                    aiMode = HUNT; // Return to HUNT mode after sinking the ship
                    aiPotentialTargets.clear(); // Clear potential targets
                    aiHits.clear(); // Clear hits for this ship
                }

                if (playerBoard.allShipsSunk()) {
                    std::cout << "\nAI won! You lose. who is the best fighter?\n";
                    gameOver = true;
                    break;
                }
                // AI gets another turn on hit
            } else {
                std::cout << "AI missed.\n";
                aiTurn = false; // End AI's turn on a miss
            }
        }
    }

    // Display final statistics
    playerBoard.display(aiBoard, true, true);
    std::cout << "\n=== Game Over ===\n";
    std::cout << "Your moves: " << playerMoves << "\n";
    std::cout << "AI's moves: " << aiMoves << "\n";
    std::cout << "Ships you destroyed: " << playerDestroyedShips << "\n";
    std::cout << "Ships AI destroyed: " << aiDestroyedShips << "\n";

    return 0;
}
