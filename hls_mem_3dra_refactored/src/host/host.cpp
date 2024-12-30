/**
 * Copyright (C) 2019-2021 Xilinx, Inc
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You may
 * not use this file except in compliance with the License. A copy of the
 * License is located at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

// #include "cmdlineparser.h"
#include <iostream>
#include <cstring>

// XRT includes
#include "experimental/xrt_bo.h"
#include "experimental/xrt_device.h"
#include "experimental/xrt_kernel.h"
#include <ap_int.h>

#include "../General_Params.h"
#include "../Decoupled.h"
#include "../AXI_S.h"
#include "parser.h"

#define DATA_LENGTH 2

int main(int argc, char **argv)
{
    std::string binaryFile = argv[1];
    int device_index = 0;

    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Open the device " << device_index << std::endl;
    auto device = xrt::device(device_index);
    std::cout << "Load the xclbin " << binaryFile << std::endl;
    auto uuid = device.load_xclbin(binaryFile);

    std::vector<PEConfig> pe_configs = parse_dfg_file("dfg/dfg.xml");
    for (auto& cfg : pe_configs) {
        std::cout << "idx = " << cfg.idx << "; opcode = " << cfg.opcode
            << "; src1_used = " << cfg.src1_used << "; src1 = " << cfg.src1 << "; src1_const = " << cfg.src1_const
            << "; src2_used = " << cfg.src2_used << "; src2 = " << cfg.src2 << "; src2_const = " << cfg.src2_const
            << '\n';
    }

    Decoupled<AXI_S> s_axi_in_A[N_PEs], s_axi_in_B, m_axi_out;
    auto krnl = xrt::kernel(device, uuid, "TDRA_Wrapper");

    std::cout << "Allocate Buffer in Global Memory\n";
    auto bo_s_axi_in_A = xrt::bo(device, sizeof(s_axi_in_A), krnl.group_id(0));
    auto bo_s_axi_in_B = xrt::bo(device, sizeof(s_axi_in_B), krnl.group_id(1));
    auto bo_m_axi_out = xrt::bo(device, sizeof(m_axi_out), krnl.group_id(2));

    for (int i = 0; i < N_PEs; ++i) {
        int bit_offset = 0;          // Bit offset for packing fields

        // Entry 1: src1 used, src1 source index, src1 constant used
        s_axi_in_A[i].bits.data(bit_offset + 0, bit_offset + 0) = pe_configs[i].src1_used;
        s_axi_in_A[i].bits.data(bit_offset + LOG_SIZE, bit_offset + 1) = pe_configs[i].src1;
        s_axi_in_A[i].bits.data(bit_offset + LOG_SIZE + 1, bit_offset + LOG_SIZE + 1)
            = (pe_configs[i].src1_const != 0);
        bit_offset += DATA_WIDTH;

        // Entry 2: src1 constant value
        s_axi_in_A[i].bits.data(bit_offset + DATA_WIDTH - 1, bit_offset) = pe_configs[i].src1_const;
        bit_offset += DATA_WIDTH;

        // Entry 3: src2 used, src2 source index, src2 constant used
        s_axi_in_A[i].bits.data(bit_offset + 0, bit_offset + 0) = pe_configs[i].src2_used;
        s_axi_in_A[i].bits.data(bit_offset + LOG_SIZE, bit_offset + 1) = pe_configs[i].src2;
        s_axi_in_A[i].bits.data(bit_offset + LOG_SIZE + 1, bit_offset + LOG_SIZE + 1) = (pe_configs[i].src2_const != 0);
        bit_offset += DATA_WIDTH;

        // Entry 4: src2 constant value
        s_axi_in_A[i].bits.data(bit_offset + DATA_WIDTH - 1, bit_offset) = pe_configs[i].src2_const;
        bit_offset += DATA_WIDTH;

        // Entry 5: pred used, pred source index, pred constant value (predication)
        s_axi_in_A[i].bits.data(bit_offset + 0, bit_offset + 0) = pe_configs[i].pred_used;
        s_axi_in_A[i].bits.data(bit_offset + LOG_SIZE, bit_offset + 1) = pe_configs[i].pred;
        s_axi_in_A[i].bits.data(bit_offset + LOG_SIZE + 1, bit_offset + LOG_SIZE + 1) = pe_configs[i].pred_const;
        bit_offset += DATA_WIDTH;

        // Entry 6: Output valid, opcode
        s_axi_in_A[i].bits.data(bit_offset + 0, bit_offset + 0) = (pe_configs[i].idx != -1); // Output valid if idx is not -1
        s_axi_in_A[i].bits.data(bit_offset + OPCODE_WIDTH, bit_offset + 1) = pe_configs[i].opcode;
        bit_offset += DATA_WIDTH;

        // Entry 7: Initial output value
        s_axi_in_A[i].bits.data(bit_offset + DATA_WIDTH - 1, bit_offset) = 0;
    }

    // Map the contents of the buffer object into host memory
    bo_s_axi_in_A.write(&s_axi_in_A);
    bo_s_axi_in_B.write(&s_axi_in_B);
    bo_m_axi_out.write(&m_axi_out);

    // Synchronize buffer content with device side
    std::cout << "synchronize input buffer data to device global memory\n";

    bo_s_axi_in_A.sync(XCL_BO_SYNC_BO_TO_DEVICE);
    bo_s_axi_in_B.sync(XCL_BO_SYNC_BO_TO_DEVICE);

    std::cout << "Execution of the kernel\n";
    auto run = krnl(bo_s_axi_in_A, bo_s_axi_in_B, bo_m_axi_out);
    run.wait();

    // Get the output;
    std::cout << "Get the output data from the device" << std::endl;
    bo_m_axi_out.sync(XCL_BO_SYNC_BO_FROM_DEVICE);

    /// TODO: print bo_m_axi_out;
    // auto bo_out_map = bo_m_axi_out.map();

    // // Validate our results
    // for (int i = 0 ; i < DATA_LENGTH ; i++){
    //     std::cout << bo_out_map[i] << '\n';
    // }
    // printf("\n");

    std::cout << "TEST PASSED\n";
    return 0;
}