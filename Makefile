PLATFORM  ?= W800
BUILD_DIR := build/$(PLATFORM)
ELF       := $(BUILD_DIR)/$(PLATFORM)_stub.elf
ASM_OUT   := $(BUILD_DIR)/$(PLATFORM)_stub.asm
BIN       := $(BUILD_DIR)/$(PLATFORM)_stub_code.bin
FINAL     := $(BUILD_DIR)/$(PLATFORM)_Stub.bin

ifeq ($(strip $(PLATFORM)),)
$(error "Platform is not selected, you must run: make PLATFORM=<name>" )
endif

ifneq ($(wildcard platforms/$(PLATFORM)/Makefile),)
include platforms/$(PLATFORM)/Makefile
else
$(error No makefile found for platform '$(PLATFORM)')
endif

CC := $(TOOLCHAIN)$(CROSS_COMPILE)gcc
OBJCOPY := $(TOOLCHAIN)$(CROSS_COMPILE)objcopy
OBJDUMP := $(TOOLCHAIN)$(CROSS_COMPILE)objdump
SIZE := $(TOOLCHAIN)$(CROSS_COMPILE)size
PYTHON ?= python3
HOST_CC ?= gcc


MINIZ_FLAGS := -DTDEFL_LESS_MEMORY=1 -DMINIZ_NO_MALLOC -DMINIZ_NO_STDIO -DMINIZ_NO_TIME -DMINIZ_NO_ARCHIVE_APIS -DMINIZ_NO_ZLIB_COMPATIBLE_NAMES -DNDEBUG
SHARED_FLAGS += -Isrc -Ithird_party/miniz -Iplatforms/$(PLATFORM) $(MINIZ_FLAGS) -DPLATFORM_$(PLATFORM)=1
CFLAGS += $(SHARED_FLAGS)
LDFLAGS += $(SHARED_FLAGS) -Wl,--print-memory-usage

SRCS_C += src/flasher_stub.c
SRCS_C += src/stub_miniz.c
SRCS_C += src/libc.c
SRCS_C += third_party/miniz/miniz_tdef.c
SRCS_C += third_party/miniz/miniz_tinfl.c
SRCS_C += third_party/miniz/miniz.c
SRCS_C += platforms/hal_generic.c

OBJS := $(SRCS_S:%.S=$(BUILD_DIR)/%.o)
OBJS += $(SRCS_C:%.c=$(BUILD_DIR)/%.o)

all: $(FINAL)

$(BUILD_DIR):
	@mkdir -p $(dir $@)

$(BUILD_DIR)/%.o : %.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	@echo "build $<"
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o : %.S | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	@echo "build $<"
	@$(CC) $(CFLAGS) -c $< -o $@

$(ELF): $(OBJS)
	@echo "link $<"
	@$(CC) $(LDFLAGS) $^ -o $@
	@$(SIZE) $@

$(BIN): $(ELF)
	@$(OBJCOPY) -O binary $< $@

clean:
	rm -rf build

.PHONY: all clean
