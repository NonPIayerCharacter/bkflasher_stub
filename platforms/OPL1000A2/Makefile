CROSS_COMPILE ?= arm-none-eabi-

CFLAGS := -mcpu=cortex-m3 -mthumb -flto -Os -fdata-sections -ffunction-sections -std=gnu2x -mthumb -mlittle-endian -DPLATFORM_PROVIDES_LIBC -nostdlib -nostartfiles -ffreestanding
LDFLAGS := -mcpu=cortex-m3 -mthumb -flto -Wl,--gc-sections -Wl,-Tplatforms/$(PLATFORM)/stub.ld -Wl,-Map=$(BUILD_DIR)/$(PLATFORM)_stub.map -Wl,--cref -nostdlib -nostartfiles -ffreestanding

SRCS_C += platforms/OPL1000A2/opl1000a2_hal.c

$(FINAL): $(BIN)
	@echo "Producing stub $(FINAL)"
	@gzip -k $(BIN)
	@mv $(BIN).gz $(FINAL)
