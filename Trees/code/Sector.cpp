#include "Sector.h"
#include <cmath>
#include <iostream>

// Constructor implementation

Sector::Sector(int x, int y, int z) : x(x), y(y), z(z), left(nullptr), right(nullptr), parent(nullptr), color(RED) {
        // TODO: Calculate the distance to the Earth, and generate the sector code
        distance_from_earth=(std::sqrt(x * x + y * y + z * z));


        std::string xCode;
        std::string yCode;
        std::string zCode;

        if (x==0){xCode="S";}
        else{ xCode = this->x>0 ? "R" : "L" ;}

        if (y==0){yCode="S";}
        else{ yCode = this->y>0 ? "U" : "D" ;}

        if (z==0){zCode="S";}
        else{ zCode = this->z>0 ? "F" : "B" ;}
        sector_code=std::to_string(static_cast<int>(distance_from_earth)) + xCode + yCode + zCode;


}

Sector::~Sector() {
    // TODO: Free any dynamically allocated memory if necessary
}

Sector& Sector::operator=(const Sector& other) {
    if (this!=&other){
        x = other.x;
        y = other.y;
        z = other.z;
        distance_from_earth = other.distance_from_earth;
        sector_code = other.sector_code;

        color=other.color;
        left = other.left;
        right = other.right;
        parent = other.parent;

    }
    return *this;
}

bool Sector::operator==(const Sector& other) const {
    return (x == other.x && y == other.y && z == other.z);
}

bool Sector::operator!=(const Sector& other) const {
    return !(*this == other);
}