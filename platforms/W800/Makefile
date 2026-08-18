TOOLCHAIN ?= platforms/W800/csky/bin/
CROSS_COMPILE ?= csky-abiv2-elf-

CFLAGS := -mcpu=ck804ef -mhard-float -Os -std=gnu99 -ffunction-sections -fdata-sections -fno-builtin -nostdlib -nodefaultlibs -nostartfiles
LDFLAGS := -mcpu=ck804ef -mhard-float -nostdlib -nodefaultlibs -nostartfiles -Wl,--gc-sections -Wl,-Tplatforms/$(PLATFORM)/stub.ld -Wl,-Map=$(BUILD_DIR)/$(PLATFORM)_stub.map

IMG := $(BUILD_DIR)/$(PLATFORM)_Stub.img

SRCS_S = platforms/W800/start.S
SRCS_C += platforms/W800/w800_hal.c

$(FINAL): prebuild $(BIN) platforms/W800/make_w800_image.py
	@echo "Producing stub $<"
	@$(PYTHON) platforms/W800/make_w800_image.py $(BIN) $(IMG) $(FINAL)

prebuild:
	@if [ ! -e platforms/W800/csky/bin ]; then cd platforms/W800/csky && tar zxf csky-elf-noneabiv2-tools-x86_64-newlib-20250328.tar.gz > /dev/null; fi
