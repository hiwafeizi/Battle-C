// Ship.h
#ifndef SHIP_H
#define SHIP_H

#include <string>
#include <vector>

// Represents a ship in the game
class Ship {
private:
    std::string name;                             // Name of the ship
    int size;                                     // Size (length) of the ship
    std::vector<std::pair<int, int>> coordinates; // Coordinates occupied by the ship
    int hits;                                     // Number of times the ship has been hit

public:
    // Constructor
    Ship(std::string name, int size);

    // Adds a coordinate to the ship's position
    void addCoordinate(int x, int y);

    // Checks if the ship occupies a specific coordinate
    bool occupiesCoordinate(int x, int y) const;

    // Registers a hit on the ship
    void hit();

    // Checks if the ship is sunk
    bool isSunk() const;

    // Gets the name of the ship
    std::string getName() const;

    // Gets the size of the ship
    int getSize() const;

    // Gets the coordinates occupied by the ship
    const std::vector<std::pair<int, int>>& getCoordinates() const;
};

#endif // SHIP_H
