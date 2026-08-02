CROSS_COMPILE ?= arm-none-eabi-

CFLAGS := -mcpu=cortex-m4 -mthumb -flto -Os -std=gnu99 -mthumb -mlittle-endian -fno-strict-aliasing -DPLATFORM_PROVIDES_LIBC
LDFLAGS := -mcpu=cortex-m4 -mthumb -flto -Wl,--gc-sections -Wl,-Tplatforms/$(PLATFORM)/stub.ld -Wl,-Map=$(BUILD_DIR)/$(PLATFORM)_stub.map

SRCS_C += platforms/RDA5981/rda5981_hal.c

$(FINAL): $(BIN)
	@echo "Producing stub $(FINAL)"
	@gzip -k $(BIN)
	@mv $(BIN).gz $(FINAL)
