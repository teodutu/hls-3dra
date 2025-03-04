
# TARGET := hw_emu
TARGET := sw_emu
PLATFORM := xilinx_u250_gen3x16_xdma_4_1_202210_1
# PLATFORM := xilinx_vck5000_gen4x8_xdma_2_202210_1
CURRDIR := $(shell pwd)
SRCDIR := $(CURDIR)/src
BUILD_DIR := $(TARGET)

CXXFLAGS += -g -std=c++1y -Wall -O0

VPP_OPTS := -t ${TARGET} --platform $(PLATFORM)
VPP_OPTS += --kernel_frequency 0:650 --hls.clock=650000000:tdra_vitis

run: build
ifeq ($(TARGET),hw)
	cp xrt.ini $(BUILD_DIR)
	cd $(BUILD_DIR) && ./app.exe tdra.xclbin
else
	cp xrt.ini $(BUILD_DIR)
	cd $(BUILD_DIR) && XCL_EMULATION_MODE=$(TARGET) ./app.exe tdra.xclbin
endif

build: host emconfig xclbin
#build: $(BUILD_DIR)/app.exe $(BUILD_DIR)/emconfig.json $(BUILD_DIR)/tdra.xclbin

host: $(BUILD_DIR)/app.exe
$(BUILD_DIR)/app.exe:
	mkdir -p $(BUILD_DIR)
	g++ $(CXXFLAGS) $(SRCDIR)/host/host.cpp -o $(BUILD_DIR)/app.exe \
		-I${XILINX_XRT}/include/ -I${XILINX_HLS}/include\
                -L$(XILINX_XRT)/lib -lOpenCL -lxrt_coreutil -lrt -luuid -pthread

xo: $(BUILD_DIR)/tdra.xo
$(BUILD_DIR)/tdra.xo:
	v++ $(VPP_OPTS) --config $(SRCDIR)/config.cfg -c -k tdra_vitis -I$(SRCDIR) $(SRCDIR)/*.cpp -o $(BUILD_DIR)/tdra.xo 

xclbin: $(BUILD_DIR)/tdra.xclbin
$(BUILD_DIR)/tdra.xclbin: $(BUILD_DIR)/tdra.xo
	v++ $(VPP_OPTS) -l --config $(SRCDIR)/config.cfg $(BUILD_DIR)/tdra.xo -o $(BUILD_DIR)/tdra.xclbin

emconfig: $(BUILD_DIR)/emconfig.json
$(BUILD_DIR)/emconfig.json:
	emconfigutil --platform $(PLATFORM) --od $(BUILD_DIR) --nd 1

clean:
	rm -rf $(BUILD_DIR) tdra* app.exe *json opencl* *log *summary _x xilinx* .run .Xil .ipcache *.jou

# Unless specified, use the current directory name as the v++ build target
#TARGET ?= $(notdir $(CURDIR))
