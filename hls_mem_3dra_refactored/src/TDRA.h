#ifndef TDRA_H
#define TDRA_H

#include "General_Params.h"
#include "Decoupled.h"
#include "Opcode.h"
#include "Store_Req.h"
#include "PE.h"
#include "MEM_CTRL.h"
#include <ap_int.h>
#include <array>

struct TDRA_IO {
    bool done;

    Decoupled<ap_uint<MEM_WIDTH>> mem_in1;
    Decoupled<ap_uint<MEM_WIDTH>> mem_in2;
    Decoupled<ap_uint<MEM_WIDTH>> mem_out;
    Decoupled<ap_uint<LOG_SIZE>> rst;
};

void TDRA(TDRA_IO &io);

#endif // TDRA_H
