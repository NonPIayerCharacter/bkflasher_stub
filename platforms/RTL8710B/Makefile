CROSS_COMPILE ?= arm-none-eabi-

CFLAGS := -mcpu=cortex-m4 -mthumb -flto -Os -std=gnu2x -mthumb -mlittle-endian -fno-strict-aliasing -DPLATFORM_PROVIDES_LIBC -nostdlib -nostartfiles -ffreestanding
LDFLAGS := -mcpu=cortex-m4 -mthumb -flto -Wl,--gc-sections -Wl,-Tplatforms/$(PLATFORM)/stub.ld -Wl,-Map=$(BUILD_DIR)/$(PLATFORM)_stub.map -Wl,--cref -Wl,--entry=Reset_Handler -nostdlib -nostartfiles -ffreestanding

SRCS_C += platforms/RTL8710B/rtl8710b_hal.c

$(FINAL): $(BIN)
	@echo "Producing stub $(FINAL)"
	@gzip -k $(BIN)
	@mv $(BIN).gz $(FINAL)
