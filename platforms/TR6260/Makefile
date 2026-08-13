TOOLCHAIN ?= platforms/ECR6600/nds32le-elf/bin/
CROSS_COMPILE ?= nds32le-elf-

CFLAGS := -flto -Os -std=gnu2x -nostartfiles -nostdlib -ffreestanding -ffunction-sections -fdata-sections -DPLATFORM_PROVIDES_LIBC -fipa-pta -mrelax -DNO_MINIZ_COMPRESSION -DNO_MINIZ_DECOMPRESSION
LDFLAGS := -flto -nostartfiles -nostdlib -ffreestanding -Wl,--gc-sections -Wl,-Tplatforms/$(PLATFORM)/stub.ld -Wl,-Map=$(BUILD_DIR)/$(PLATFORM)_stub.map -Wl,--no-warn-rwx-segments -z noexecstack
#LDFLAGS += -fPIC

IMG := $(BUILD_DIR)/$(PLATFORM)_Stub.img

SRCS_S = platforms/ECR6600/start.S
SRCS_C += platforms/TR6260/tr6260_hal.c

$(FINAL): prebuild $(BIN)
	@echo "Producing stub $<"
	@$(PYTHON) platforms/TR6260/generate-tr6260-header.py $(BIN) --addr 0x1C000 -o $(IMG)
	@gzip -k $(IMG)
	@mv $(IMG).gz $(FINAL)

prebuild:
	@if [ ! -e platforms/ECR6600/nds32le-elf ]; then cd platforms/ECR6600 && xz -d < nds32le-elf.tar.xz | tar xvf - > /dev/null; fi
