#ifndef DECOUPLED_H
#define DECOUPLED_H

#include <ap_int.h>

template <typename T>
struct Decoupled {
    T bits;
    bool valid;
    bool ready;

    // Default constructor
    Decoupled() : bits(0), valid(false), ready(false) {}

    // Copy constructor (optional)
    Decoupled(const Decoupled &other) : bits(other.bits), valid(other.valid), ready(other.ready) {}

    // Assignment operator
    Decoupled& operator=(const Decoupled &other) {
        bits = other.bits;
        valid = other.valid;
        ready = other.ready;
        return *this;
    }

    bool fire() {
        return valid && ready;
    }
};

#endif // DECOUPLED_H
