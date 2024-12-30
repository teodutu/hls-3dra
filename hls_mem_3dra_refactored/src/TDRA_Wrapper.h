#ifndef TDRA_WRAPPER_H
#define TDRA_WRAPPER_H

#include <ap_int.h>
#include <hls_stream.h>

#include "AXI_S.h"
#include "Decoupled.h"
#include "General_Params.h"
#include "TDRA.h"

// Function prototype for FPGA Kernel
extern "C" void TDRA_Wrapper(Decoupled<AXI_S>* s_axi_in_A, Decoupled<AXI_S>* s_axi_in_B, Decoupled<AXI_S>* m_axi_out);

#endif  // TDRA_WRAPPER_H
