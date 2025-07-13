all: build run

sim:
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!
	@echo "Sim successfully"


# Verilator 
VERILATOR = verilator

#
TA = tao_top
TARGET = V$(TA)

# Verilog  C++ 
NPC_HOME = /home/kaka/ysyx-workbench/npc
VSRCDIR = $(NPC_HOME)/vsrc
CSRC = $(NPC_HOME)/csrc

# Verilog（递归查找所有 .v）
SRC = $(shell find $(VSRCDIR) -name "*.v")

# C++（递归查找所有 .cpp）
CPP_SRC = $(shell find $(CSRC) -name "*.cpp" ! -path "*/tools/*")
C_SRC = $(shell find $(CSRC) -name "*.c" ! -path "*/tools/*")

# Verilator 
OBJ_DIR = obj_dir

# 
VCD_FILE = dump.vcd

# 
CFLAGS = -Wall -g
CFLAGS += -I$(CSRC)/include -I$(CSRC)/include/sdb -I$(CSRC)/include/cpu 

LDFLAGS += -lreadline

#include
include $(CSRC)/src/utils/filelist.mk

#  Verilator  Verilog 
build:
	$(VERILATOR) -cc -exe -build \
	-I$(VSRCDIR) -I$(VSRCDIR)/core -I$(VSRCDIR)/general \
	-CFLAGS "$(CFLAGS)" \
	-LDFLAGS "$(LDFLAGS)" \
	-top-module $(TA) -trace $(SRC) $(CPP_SRC) $(C_SRC)

# 
run:
	@echo "Running simulation..."
	./$(OBJ_DIR)/$(TARGET) $(IMG) $(ELF)

# 
wave:
	@echo "Opening waveform..."
	gtkwave $(VCD_FILE) &

# 
clean:
	rm -rf $(OBJ_DIR) *.log
	rm dump.vcd

# 
.PHONY: all build run clean

