#include "Ship.h"

// Constructor initializes the ship's name and size
Ship::Ship(std::string name, int size) : name(name), size(size), hits(0) {}

// Adds a coordinate to the ship's position
void Ship::addCoordinate(int x, int y) {
    coordinates.push_back({x, y});
}

// Checks if the ship occupies a specific coordinate
bool Ship::occupiesCoordinate(int x, int y) const {
    for (const auto& coord : coordinates) {
        if (coord.first == x && coord.second == y)
            return true;
    }
    return false;
}

// Registers a hit on the ship
void Ship::hit() {
    hits++;
}

// Checks if the ship is sunk
bool Ship::isSunk() const {
    return hits >= size;
}

// Gets the name of the ship
std::string Ship::getName() const {
    return name;
}

// Gets the size of the ship
int Ship::getSize() const {
    return size;
}

// Gets the coordinates occupied by the ship
const std::vector<std::pair<int, int>>& Ship::getCoordinates() const {
    return coordinates;
}
