TOOLCHAIN ?= /mnt/data/csky_toolchain/bin/
CROSS_COMPILE ?= csky-abiv2-elf-

CFLAGS := -mcpu=ck804ef -mhard-float -Os -std=gnu99 -ffunction-sections -fdata-sections -fno-builtin -nostdlib -nodefaultlibs -nostartfiles
LDFLAGS := -mcpu=ck804ef -mhard-float -nostdlib -nodefaultlibs -nostartfiles -Wl,--gc-sections -Wl,-Tplatforms/$(PLATFORM)/stub.ld -Wl,-Map=$(BUILD_DIR)/$(PLATFORM)_stub.map

IMG := $(BUILD_DIR)/$(PLATFORM)_Stub.img

SRCS_S = platforms/W800/start.S
SRCS_C += platforms/W800/w800_hal.c

$(FINAL): $(BIN) platforms/W800/make_w800_image.py
	@echo "Producing stub $<"
	@$(PYTHON) platforms/W800/make_w800_image.py $(BIN) $(IMG) $(FINAL)
