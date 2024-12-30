#ifndef AXI_S_H
#define AXI_S_H

#include <ap_int.h>

#include "General_Params.h"

// Define the AXI_S structure for AXI Stream interface
struct AXI_S {
    ap_uint<MEM_WIDTH> data;    // Data field width, based on MEM_WIDTH
    ap_uint<MEM_WIDTH / 8> keep; // Keep signal, 1 bit per byte in `data`
    bool last;                  // Last signal, indicates end of transaction

    // Default constructor
    AXI_S(ap_uint<MEM_WIDTH> val) : data(val), keep(0), last(false) {}

    // Parameterized constructor
    AXI_S(ap_uint<MEM_WIDTH> data, ap_uint<MEM_WIDTH / 8> keep, bool last)
        : data(data), keep(keep), last(last) {}

    // Assignment operator
    AXI_S& operator=(const AXI_S& other) {
        if (this != &other) {
            data = other.data;
            keep = other.keep;
            last = other.last;
        }
        return *this;
    }

    // Equality operator (useful for testing and comparison)
    bool operator==(const AXI_S& other) const {
        return (data == other.data) && (keep == other.keep) && (last == other.last);
    }
};

#endif // AXI_S_H
