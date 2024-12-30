#ifndef GENERAL_PARAMS_H
#define GENERAL_PARAMS_H


#include <cstddef>
#include <cmath>
#include <iostream>

constexpr size_t binLog2(size_t n) {
    return n < 2 ? 1 : 1 + binLog2(n >> 1);
}

// Define constants for CGRA configuration parameters
#define N_PEs               64              // Number of PEs
#define LOG_SIZE            binLog2(N_PEs)  // Bits needed for N_PEs
#define FIFO_SIZE           4               // FIFO size
#define DATA_WIDTH          32              // Data width in bits
#define MEM_WIDTH           512             // Memory width in bits
#define OPCODE_WIDTH        8               // Opcode width in bits
#define DEBUG_MODE          1               // Debug flag, set to 1 for enabled, 0 for disabled

// Macro to compute memory address width based on MEM_WIDTH
#define MEM_ADDR_WIDTH      (1 + binLog2(MEM_WIDTH))

#define SRC_FIELD_SIZE      DATA_WIDTH
#define OPCODE_FIELD_SIZE   DATA_WIDTH

#define I1_SRC_OFFSET       0
#define I1_CONST_OFFSET     (I1_SRC_OFFSET + SRC_FIELD_SIZE)
#define I2_SRC_OFFSET       (I1_CONST_OFFSET + DATA_WIDTH)
#define I2_CONST_OFFSET     (I2_SRC_OFFSET + SRC_FIELD_SIZE)
#define PRED_OFFSET         (I2_CONST_OFFSET + DATA_WIDTH)
#define OPCODE_OFFSET       (PRED_OFFSET + SRC_FIELD_SIZE)
#define INIT_OUTPUT_OFFSET  (OPCODE_OFFSET + OPCODE_FIELD_SIZE)

#define CONFIG_WIDTH        ((INIT_OUTPUT_OFFSET + DATA_WIDTH) * N_PEs)  // PE config width

#endif // GENERAL_PARAMS_H
