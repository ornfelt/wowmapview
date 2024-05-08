// WorldObject.h
#ifndef WORLDOBJECT_H
#define WORLDOBJECT_H

#include "Object.h"
#include "WorldLocation.h"

class WorldObject : public Object, public WorldLocation {
protected:
    bool isWorldObject;

public:
    WorldObject(bool isWorldObject = false) 
        : WorldLocation(), isWorldObject(isWorldObject) {}

    virtual ~WorldObject() override {}

    void describe() const override {
        std::cout << "This is a WorldObject, is it a world object? " << (isWorldObject ? "Yes" : "No") << "." << std::endl;
    }
};

#endif // WORLDOBJECT_H
