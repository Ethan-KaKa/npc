LIBCAPSTONE = $(CSRC)/tools/capstone/repo/libcapstone.so.5
CFLAGS += -I $(CSRC)/tools/capstone/repo/include
src/utils/disasm.c: $(LIBCAPSTONE)
$(LIBCAPSTONE):
	$(MAKE) -C $(CSRC)/tools/capstone CAPSTONE_BUILD_BINDINGS=no