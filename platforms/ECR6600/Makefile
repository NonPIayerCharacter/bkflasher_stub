TOOLCHAIN ?= /mnt/c/sourcecodes/openbk7231t_app/sdk/openecr6600/tool/nds32le-elf-mculib-v3s/bin/
CROSS_COMPILE ?= nds32le-elf-

CFLAGS := -flto -Os -std=gnu99 -nostartfiles -nostdlib -ffreestanding
LDFLAGS := -flto -fPIC -nostartfiles -nostdlib -ffreestanding -Wl,-Tplatforms/$(PLATFORM)/stub.ld -Wl,-Map=$(BUILD_DIR)/$(PLATFORM)_stub.map

SRCS_S = platforms/ECR6600/start.S
SRCS_C += platforms/ECR6600/ecr6600_hal.c
SRCS_C += platforms/ECR6600/ecr6600_flash.c

$(FINAL): $(BIN)
	@echo "Producing stub $(FINAL)"
	@gzip -k $(BIN)
	@mv $(BIN).gz $(FINAL)
