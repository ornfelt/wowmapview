// Unit.h
#ifndef UNIT_H
#define UNIT_H

#include "WorldObject.h"

class Unit : public WorldObject {
public:
    Unit(bool isWorldObject = false) : WorldObject(isWorldObject) {}

    virtual ~Unit() override {}

    void describe() const override {
        std::cout << "This is a Unit." << std::endl;
    }

    // Force base class behaviour
	//void describe() const override {
    //    // Call base class describe explicitly
    //    Object::describe();
    //}
};

#endif // UNIT_H
