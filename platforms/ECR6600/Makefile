TOOLCHAIN ?= platforms/ECR6600/nds32le-elf/bin/
CROSS_COMPILE ?= nds32le-elf-

CFLAGS := -flto -Os -std=gnu2x -nostartfiles -nostdlib -ffreestanding -ffunction-sections -fdata-sections -DPLATFORM_PROVIDES_LIBC -fipa-pta -mrelax #-DMINIZ_USE_UNALIGNED_LOADS_AND_STORES=1
LDFLAGS := -flto -fPIC -nostartfiles -nostdlib -ffreestanding -Wl,--gc-sections -Wl,-Tplatforms/$(PLATFORM)/stub.ld -Wl,-Map=$(BUILD_DIR)/$(PLATFORM)_stub.map -Wl,--no-warn-rwx-segments -z noexecstack

SRCS_S = platforms/ECR6600/start.S
SRCS_C += platforms/ECR6600/ecr6600_hal.c

$(FINAL): prebuild $(BIN)
	@echo "Producing stub $(FINAL)"
	@gzip -k $(BIN)
	@mv $(BIN).gz $(FINAL)

prebuild:
	@if [ ! -e platforms/ECR6600/nds32le-elf ]; then cd platforms/ECR6600 && xz -d < nds32le-elf.tar.xz | tar xvf - > /dev/null; fi
