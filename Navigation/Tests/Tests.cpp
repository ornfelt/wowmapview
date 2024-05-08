#include "Unit.h"
#include <vector>

#include "Unit.h"
#include <iostream>
#include <vector>

// Display for generic Object
void displayObject(const Object& obj) {
    std::cout << "Displaying Object: ";
    obj.describe();
}

// These two can be removed (then displayObject will be called instead)
void displayObject(const WorldObject& obj) {
    std::cout << "Displaying WorldObject: ";
    obj.describe();
}

void displayObject(const Unit& obj) {
    std::cout << "Displaying Unit: ";
    obj.describe();
}

int main() {
    Object obj;
    WorldObject wobj(true);
    Unit unit;

    // Each call will now correctly resolve to the appropriate overloaded function
    displayObject(obj);    // Calls displayObject for Object
    displayObject(wobj);   // Calls displayObject for WorldObject
    displayObject(unit);   // Calls displayObject for Unit

    // Call base method explicitly
    displayObject(static_cast<const Object&>(unit));

    return 0;
}
