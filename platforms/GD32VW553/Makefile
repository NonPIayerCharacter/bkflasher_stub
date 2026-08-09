# replace rv32imafcbp with rv32imafc if using riscv64-unknown
#CROSS_COMPILE ?= riscv64-unknown-elf-
#CFLAGS += -I/usr/include
CROSS_COMPILE ?= riscv-nuclei-elf-
TOOLCHAIN ?= /mnt/c/sourcecodes/openbk7231t_app/sdk/opengd32vw553/tools/gd32vw55x_toolchain_linux/bin/

CFLAGS := -flto -mcmodel=medlow -msmall-data-limit=8 -mabi=ilp32f -march=rv32imafcbp -Os -std=gnu2x -ffunction-sections -fdata-sections -nostartfiles -nostdlib -ffreestanding -DMINIZ_USE_UNALIGNED_LOADS_AND_STORES=1 -DEXEC_USING_STD_PRINTF
CFLAGS += -I./third_party/gd32vw55x/GD32VW55x_standard_peripheral
CFLAGS += -I./third_party/gd32vw55x/GD32VW55x_standard_peripheral/Include
CFLAGS += -I./third_party/gd32vw55x/riscv/NMSIS/Core/Include
CFLAGS += -I./third_party/gd32vw55x/riscv/gd32vw55x
CFLAGS += -I./third_party/gd32vw55x/
CFLAGS += -I./third_party/gd32vw55x/ROM-EXPORT/bootloader
CFLAGS += -I./third_party/gd32vw55x/ROM-EXPORT/halcomm
CFLAGS += -I./third_party/gd32vw55x/ROM-EXPORT/mbedtls-2.17.0-rom/include
LDFLAGS := -flto -mcmodel=medlow -msmall-data-limit=8 -msave-restore -mabi=ilp32f -march=rv32imafcbp -Wl,--gc-sections -Wl,-Tplatforms/$(PLATFORM)/stub.ld -Wl,-Map=$(BUILD_DIR)/$(PLATFORM)_stub.map -nostartfiles -nostdlib -ffreestanding -Wl,--just-symbols=third_party/gd32vw55x/ROM-EXPORT/symbol/rom_symbol_m.gcc -mno-save-restore

SRCS_C += platforms/GD32VW553/gd32vw553_hal.c
SRCS_C += third_party/gd32vw55x/GD32VW55x_standard_peripheral/Source/gd32vw55x_fmc.c
SRCS_C += third_party/gd32vw55x/GD32VW55x_standard_peripheral/Source/gd32vw55x_gpio.c
SRCS_C += third_party/gd32vw55x/GD32VW55x_standard_peripheral/Source/gd32vw55x_hau.c
SRCS_C += third_party/gd32vw55x/GD32VW55x_standard_peripheral/Source/gd32vw55x_hau_sha_md5.c
SRCS_C += third_party/gd32vw55x/GD32VW55x_standard_peripheral/Source/gd32vw55x_rcu.c
SRCS_C += third_party/gd32vw55x/GD32VW55x_standard_peripheral/Source/gd32vw55x_usart.c
SRCS_C += third_party/gd32vw55x/riscv/gd32vw55x/system_gd32vw55x.c
SRCS_C += third_party/gd32vw55x/init_rom_symbol.c
SRCS_S += third_party/gd32vw55x/riscv/env/start.S

$(FINAL): $(BIN)
	@echo "Producing stub $(FINAL)"
	@gzip -k $(BIN)
	@mv $(BIN).gz $(FINAL)
