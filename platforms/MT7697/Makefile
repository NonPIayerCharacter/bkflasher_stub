CROSS_COMPILE ?= arm-none-eabi-

CFLAGS := -mcpu=cortex-m4 -mthumb -flto -Os -std=gnu2x -mthumb -mlittle-endian -fno-strict-aliasing -DFASTER_CRC32 #-DNO_MINIZ_COMPRESSION -DNO_MINIZ_DECOMPRESSION
LDFLAGS := -mcpu=cortex-m4 -mthumb -flto -Wl,--gc-sections -Wl,-Tplatforms/$(PLATFORM)/stub.ld -Wl,-Map=$(BUILD_DIR)/$(PLATFORM)_stub.map -Wl,--cref

SRCS_C += platforms/MT7697/mt7697_hal.c

$(FINAL): $(BIN)
	@echo "Producing stub $(FINAL)"
	@gzip -k $(BIN)
	@mv $(BIN).gz $(FINAL)
