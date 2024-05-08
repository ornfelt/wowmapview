// Position.h
#ifndef POSITION_H
#define POSITION_H

class Position {
protected:
    float x, y, z, o;  // Coordinates and orientation

public:
    Position(float x = 0.0f, float y = 0.0f, float z = 0.0f, float o = 0.0f)
        : x(x), y(y), z(z), o(o) {}

    // Copy constructor
    Position(const Position& position)
        : x(position.x), y(position.y), z(position.z), o(position.o) {}

    virtual ~Position() {}
};

#endif // POSITION_H
