// Object.h
#ifndef OBJECT_H
#define OBJECT_H

#include <iostream>

class Object {
public:
    virtual ~Object() {}
    Object() {}

    virtual void describe() const {
        std::cout << "This is an Object." << std::endl;
    }
};

#endif // OBJECT_H
