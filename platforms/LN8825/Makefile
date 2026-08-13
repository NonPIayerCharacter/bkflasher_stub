CROSS_COMPILE ?= arm-none-eabi-

CFLAGS := -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -flto -Os -std=gnu99 -mthumb -mlittle-endian -fno-strict-aliasing -DLN882x -nostdlib -nostartfiles -ffreestanding
LDFLAGS := -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -flto -Wl,--gc-sections -Wl,-Tplatforms/$(PLATFORM)/stub.ld -Wl,-Map=$(BUILD_DIR)/$(PLATFORM)_stub.map -Wl,--cref -Wl,--entry=Reset_Handler -nostdlib -nostartfiles -ffreestanding -L./third_party/ln8825 -Wl,--whole-archive -ldriver_ln882x

CFLAGS += -Ithird_party/ln8825
CFLAGS += -Ithird_party/ln882h/CMSIS_5.3.0
CFLAGS += -Ithird_party/ln8825/ln88xx
CFLAGS += -Ithird_party/ln8825/driver_ln882x
CFLAGS += -Ithird_party/ln8825/driver_ln882x/inc
CFLAGS += -Ithird_party/ln8825/driver_ln882x/inc/hal
CFLAGS += -Ithird_party/ln8825/driver_ln882x/inc/ll
CFLAGS += -Ithird_party/ln8825/driver_ln882x/inc/reg

SRCS_C += platforms/LN8825/ln8825_hal.c
SRCS_C += third_party/ln8825/driver_ln882x/src/hal/cache.c
SRCS_C += third_party/ln8825/driver_ln882x/src/hal/hal_dma.c
SRCS_C += third_party/ln8825/driver_ln882x/src/hal/hal_efuse.c
SRCS_C += third_party/ln8825/driver_ln882x/src/hal/flash.c
SRCS_C += third_party/ln8825/driver_ln882x/src/hal/qspi.c
SRCS_C += third_party/ln8825/driver_ln882x/src/hal/hal_syscon.c

$(FINAL): $(BIN)
	@echo "Producing stub $(FINAL)"
	@gzip -k $(BIN)
	@mv $(BIN).gz $(FINAL)
