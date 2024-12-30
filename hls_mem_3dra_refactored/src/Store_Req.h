#ifndef STORE_REQ_H
#define STORE_REQ_H

#include "ap_int.h"
#include "General_Params.h"

struct Store_Req {
    ap_uint<MEM_ADDR_WIDTH> addr;
    ap_uint<DATA_WIDTH> data;
    bool is_store;

    Store_Req(ap_uint<DATA_WIDTH> data) : addr(0), data(data), is_store(false) { }

    Store_Req& operator=(const Store_Req& other) {
        if (this != &other) { // Prevent self-assignment
            addr = other.addr;
            data = other.data;
            is_store = other.is_store;
        }
        return *this;
    }
};

#endif  // STORE_REQ_H
