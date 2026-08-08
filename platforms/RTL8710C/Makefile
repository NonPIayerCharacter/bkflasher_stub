CROSS_COMPILE ?= arm-none-eabi-

CFLAGS := -march=armv8-m.main+dsp -mthumb -flto -Os -std=gnu99 -ffunction-sections -fdata-sections -mlittle-endian -nostdlib -nostartfiles -ffreestanding -DPLATFORM_PROVIDES_LIBC -DMINIZ_USE_UNALIGNED_LOADS_AND_STORES=1
LDFLAGS := -march=armv8-m.main+dsp -mthumb -flto -Wl,--gc-sections -Wl,-Tplatforms/$(PLATFORM)/stub.ld -Wl,-Map=$(BUILD_DIR)/$(PLATFORM)_stub.map -nostdlib -nostartfiles -ffreestanding platforms/RTL8710C/hal_crypto_lib_soc_is.o

SRCS_C += platforms/RTL8710C/rtl8710c_hal.c

$(FINAL): $(BIN)
	@echo "Producing stub $(FINAL)"
	@gzip -k $(BIN)
	@mv $(BIN).gz $(FINAL)
