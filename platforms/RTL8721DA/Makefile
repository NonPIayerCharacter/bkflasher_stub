CROSS_COMPILE ?= arm-none-eabi-

CFLAGS := -march=armv8.1-m.main+dsp -flto -mthumb -Os -std=gnu2x -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -mlittle-endian -fno-strict-aliasing -DPLATFORM_PROVIDES_LIBC -DMINIZ_USE_UNALIGNED_LOADS_AND_STORES -nostdlib -nostartfiles -ffreestanding -mfloat-abi=hard -mfpu=fpv5-sp-d16
LDFLAGS := -march=armv8.1-m.main+dsp -flto -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -Wl,--gc-sections -Wl,-Tplatforms/$(PLATFORM)/stub.ld -Wl,-Map=$(BUILD_DIR)/$(PLATFORM)_stub.map -Wl,--cref -Wl,--entry=Reset_Handler -nostartfiles -ffreestanding

SRCS_C += platforms/RTL8721DA/rtl8721da_hal.c
SRCS_C += third_party/tlsf/tlsf.c
SRCS_C += third_party/rtl_kv/kv.c
SRCS_C += third_party/littlefs/lfs.c
SRCS_C += third_party/littlefs/lfs_util.c

$(FINAL): $(BIN)
	@echo "Producing stub $(FINAL)"
	@gzip -k $(BIN)
	@mv $(BIN).gz $(FINAL)
