CROSS_COMPILE ?= arm-none-eabi-

CFLAGS := -mcpu=cortex-m4 -mthumb -flto -Os -std=gnu2x -mthumb -mlittle-endian -fno-strict-aliasing -DLN882H #-DPLATFORM_PROVIDES_LIBC -nostdlib -nostartfiles -ffreestanding
LDFLAGS := -mcpu=cortex-m4 -mthumb -flto -Wl,--gc-sections -Wl,-Tplatforms/$(PLATFORM)/stub.ld -Wl,-Map=$(BUILD_DIR)/$(PLATFORM)_stub.map -Wl,--cref -nostdlib -nostartfiles -ffreestanding

CFLAGS += -Ithird_party/ln882h
CFLAGS += -Ithird_party/ln882h/CMSIS_5.3.0
CFLAGS += -Ithird_party/ln882h/ln882h
CFLAGS += -Ithird_party/ln882h/driver_ln882h
CFLAGS += -Ithird_party/ln882h/driver_ln882h/hal
CFLAGS += -Ithird_party/ln882h/driver_ln882h/reg

SRCS_C += platforms/LN882H/ln882h_hal.c
SRCS_C += third_party/ln882h/driver_ln882h/hal/hal_cache.c
SRCS_C += third_party/ln882h/driver_ln882h/hal/hal_clock.c
SRCS_C += third_party/ln882h/driver_ln882h/hal/hal_efuse.c
SRCS_C += third_party/ln882h/driver_ln882h/hal/hal_flash.c
SRCS_C += third_party/ln882h/driver_ln882h/hal/hal_gpio.c
SRCS_C += third_party/ln882h/driver_ln882h/hal/hal_misc.c
SRCS_C += third_party/ln882h/driver_ln882h/hal/hal_qspi.c
SRCS_C += third_party/ln882h/driver_ln882h/hal/hal_uart.c

$(FINAL): $(BIN)
	@echo "Producing stub $(FINAL)"
	@gzip -k $(BIN)
	@mv $(BIN).gz $(FINAL)
