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

#define DATA_SIZE 8

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

    size_t vector_size_bytes = sizeof(ap_int<5>) * DATA_SIZE;
    size_t vector_size_bytes_2 = sizeof(ap_int<32>) * DATA_SIZE;

    auto krnl = xrt::kernel(device, uuid, "tdra_vitis");

    std::cout << "Allocate Buffer in Global Memory\n";
    auto bo0 = xrt::bo(device, vector_size_bytes, krnl.group_id(0));
    auto bo1 = xrt::bo(device, vector_size_bytes, krnl.group_id(1));
    auto bo2 = xrt::bo(device, vector_size_bytes, krnl.group_id(2));
    auto bo_out = xrt::bo(device, vector_size_bytes_2, krnl.group_id(3));

    ap_int<5> i1_src[DATA_SIZE] = {0, 1, 0, 1, 1, 0, 1, 0};
    ap_int<5> i2_src[DATA_SIZE] = {1, 0, 1, 0, 1, 1, 0, 1};
    ap_int<5> opcode[DATA_SIZE] = {0, 1, 1, 2, 0, 2, 0, 0};
    ap_int<32> output[DATA_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0};
    // ap_int<5> i1_src[DATA_SIZE] = {0, 1};
    // ap_int<5> i2_src[DATA_SIZE] = {1, 0};
    // ap_int<5> opcode[DATA_SIZE] = {0, 1};
    // ap_int<32> output[DATA_SIZE] = {0, 0}; 

        // {7, 12, 12, 1, 8, -1, 7,7};

    // bo0.write(i1_src);
    // bo1.write(i2_src);
    // bo2.write(opcode);
    // bo_out.write(output);


    // Map the contents of the buffer object into host memory
    bo0.write(i1_src);
    bo1.write(i2_src);
    bo2.write(opcode);
    // bo_out.write(output);

    auto bo_out_map = bo_out.map<ap_int<32> *>();
    std::fill(bo_out_map, bo_out_map + DATA_SIZE, 0);

    // Create the test data
    ap_int<32> bufReference[DATA_SIZE]={7, 12, 84, 5, 10, 79, 89, 89};




    // Synchronize buffer content with device side
    std::cout << "synchronize input buffer data to device global memory\n";

    bo0.sync(XCL_BO_SYNC_BO_TO_DEVICE);
    bo1.sync(XCL_BO_SYNC_BO_TO_DEVICE);
    bo2.sync(XCL_BO_SYNC_BO_TO_DEVICE);

    std::cout << "Execution of the kernel\n";
    auto run = krnl(bo0, bo1, bo2, bo_out, DATA_SIZE);
    run.wait();

    // Get the output;
    std::cout << "Get the output data from the device" << std::endl;
    bo_out.sync(XCL_BO_SYNC_BO_FROM_DEVICE);

    // Validate our results
    for (int i = 0 ; i < DATA_SIZE ; i++){    
            std::cout << bo_out_map[i] << std::endl;
    }
    
    if (std::memcmp(bo_out_map, bufReference, DATA_SIZE)){
        throw std::runtime_error("Value read back does not match reference");
    }
    else{
        std::cout << "TEST PASSED\n";
    }
    


    
    return 0;
}