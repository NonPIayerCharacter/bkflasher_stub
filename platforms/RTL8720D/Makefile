CROSS_COMPILE ?= arm-none-eabi-

CFLAGS := -march=armv8-m.base -mthumb -flto -Os -std=gnu99 -mthumb -mlittle-endian -fno-strict-aliasing -DPLATFORM_PROVIDES_LIBC -nostdlib -nostartfiles -ffreestanding -fno-jump-tables -DNO_MINIZ_COMPRESSION -DFASTER_CRC32
LDFLAGS := -march=armv8-m.base -mthumb -flto -Wl,--gc-sections -Wl,-Tplatforms/$(PLATFORM)/stub.ld -Wl,-Map=$(BUILD_DIR)/$(PLATFORM)_stub.map -Wl,--cref -Wl,--entry=Reset_Handler -nostdlib -nostartfiles -ffreestanding

SRCS_C += platforms/RTL8720D/rtl8720d_hal.c

$(FINAL): $(BIN)
	@echo "Producing stub $(FINAL)"
	@gzip -k $(BIN)
	@mv $(BIN).gz $(FINAL)
