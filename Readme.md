
# HLS 3DRA

This folder contains 3 projects.
- hirunas_2pe 
    - 3DRA implementation without local memory. 
    - Capable of handling computations like add, minus, mult, and etc.
    - In the pe.cpp , the first pair of operands are hardcoded there, operation starts from those two numbers
- hls_3dra
    - A re-implementation of hirunas version to allow ease of scaling up the design
- hls_mem_3dra
    - Build on top of the hls_3dra with memory support. 
    - Does compile BUT this version is not tested !!!
    - TODO
        - handle constant 
        - load memory from host to kernel local memory before processing instructions >> which will replace the in_channel initializaion in tdra_vitis.h
        - Current II of the TDRA is more than 1, to fix it , each PE should has its own channel input (https://support.xilinx.com/s/question/0D52E00006iHsrISAS/how-to-set-the-two-functions-can-be-run-parallel?language=en_US)

**NOTE!**: To match the original 3DRA, logics to handle constant, predict and select (if required) are need to be added.

## Table of Contents

- [Sythesizing on FPGA](#Sythesizing_on_FPGA)
- [Clocks](#Clocks)

## Sythesizing on FPGA

Transfer folder to HACC Cluster using this command.

    scp -r ./(folder that need to transfer) xxxxx@xacchead.d2.comp.nus.edu.sg:/home/hiruna/

Log in to the HACC Cluster using the following command here xxxxx is the user id.

    ssh xxxxx@xacchead.d2.comp.nus.edu.sg

### To sythesize on U250

Run ALL of the following command to add Vitis to the environment

    source /tools/Xilinx/Vitis/2021.2/settings64.sh
    source /tools/Xilinx/Vivado/2021.2/settings64.sh
    source /tools/Xilinx/Vitis_HLS/2021.2/settings64.sh
    source /opt/xilinx/xrt/setup.sh

To generate the xclbin file that required to synthesized the architecture on U250 use this command. For details refer to Makefile

    make clean
    make run TARGET=hw DEVICE=/opt/xilinx/platforms/xilinx_u250_gen3x16_xdma_3_1_202020_1/xilinx_u250_gen3x16_xdma_3_1_202020_1.xpfm
    **change TARGET to hw_emu or sw_emu to run emulation during the development process.

Setup Slurm Interactive : https://github.com/Xtra-Computing/hacc_demo/blob/main/doc/1-FPGA-allocation.md

    shell command : srun -p u250_standard_reservation_pool --cpus-per-task=4  --pty bash -i
    
    program partition: sudo /opt/xilinx/xrt/bin/xbmgmt program --shell /opt/xilinx/firmware/u250/gen3x16/xdma-shell/partition.xsabin  --device

    reset if required
    - xbutil examine
    - xbutil reset - d divice_number

Important : U250 platform file (`.xpfm`) only works with Vitis version 2021.2
vck5000 platfrom file only works with Vitis version 2022.1

### To sythesize on vck5000

Run the following command to add Vitis to the environment

    source /tools/Xilinx/Vitis/2022.1/settings64.sh
    source /tools/Xilinx/Vivado/2022.1/settings64.sh
    source /tools/Xilinx/Vitis_HLS/2022.1/settings64.sh
    source /opt/xilinx/xrt/setup.sh

Change `DEVICE:= xilinx_vck5000_gen4x8_xdma_2_202210_1`, `XCLBIN` to `XSA` and `xclbin` to `xsa` in Makefile.

To generate the xsa file that required to synthesized the architecture on vck5000 use this command.

    make run TARGET=hw DEVICE=/opt/xilinx/platforms/xilinx_vck5000_gen4x8_xdma_2_202210_1/xilinx_vck5000_gen4x8_xdma_2_202210_1.xpfm

To view the final result, nevigate to the project directory and run the following command.

    vitis_analyzer .

`System Estimate` will show the frequency results.

However to view it in GUI, VNC capability must be established. To do that use the following tutorial.

    https://github.com/XACCNUS/Job-Scheduler/blob/main/vnc_setup.md

## Clocks

To change the clock frequency that hls synthesized at, use the following command in the Makefile. Here `tdra_vtis` is name of the to kernel. This clock is set to 750MHz.

    --hls.clock=750000000:tdra_vitis

To change the frequency on hardware use the following command in the Makefile. This clock is set to 650MHz.

    --kernel_frequency 0:650

Important : When adding new libraries to the host program make sure to include them in the Makefile to `CFLAGS` variable e.g `-I$(XILINX_XRT)/include -I${XILINX_HLS}/include`
