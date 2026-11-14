CROSS_COMPILE ?= arm-none-eabi-

CFLAGS := -mcpu=cortex-m3 -mthumb -O1 -flto -ffreestanding -fno-builtin -fdata-sections -ffunction-sections -nostdlib -DNO_MINIZ_COMPRESSION -DNO_MINIZ_DECOMPRESSION -DFASTER_CRC32 -DLIGHT_MEMCPY -DLIGHT_MEMSET
LDFLAGS := -mcpu=cortex-m3 -mthumb -O1 -flto -ffreestanding -fno-builtin -fdata-sections -ffunction-sections -nostdlib -Wl,--gc-sections

SRCS_S += platforms/W600/startup.S
SRCS_C += platforms/W600/w600_hal.c

COPY_STUB_OBJ := $(BUILD_DIR)/secboot-copy-stub.o
COPY_STUB_BIN := $(BUILD_DIR)/secboot-copy-stub.bin
W600_OBJS := $(SRCS_S:%.S=$(BUILD_DIR)/%.o) $(SRCS_C:%.c=$(BUILD_DIR)/%.o)

$(FINAL): $(BIN) $(COPY_STUB_BIN) platforms/W600/make_w600_image.py
	@echo "Producing stub $(FINAL)"
	@$(PYTHON) platforms/W600/make_w600_image.py $(BIN) $(COPY_STUB_BIN) $@

$(COPY_STUB_OBJ): platforms/W600/secboot_copy_stub.S | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	@$(CC) -c -mcpu=cortex-m3 -mthumb $< -o $@

$(COPY_STUB_BIN): $(COPY_STUB_OBJ)
	@$(OBJCOPY) -O binary -j .text $< $@

$(W600_OBJS): platforms/W600/Makefile platforms/W600/stub.ld

.PHONY: prebuild
prebuild:
