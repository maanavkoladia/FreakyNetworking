# See: https://makefiletutorial.com/
# Cross-compiler and flags
MODE=sw

ifeq ($(MODE), hw)
	BUILD_DIR = build/hw

	CC = arm-none-eabi-gcc
	OBJCOPY = arm-none-eabi-objcopy

	# Simulator mode
	CPU_FLAGS = \
    -mcpu=cortex-m4

	FPU_FLAGS = \
		-mfpu=fpv4-sp-d16 \
		-mfloat-abi=hard

	DEFINES = \
		-DPART_TM4C123GH6PM \
		-DTARGET_IS_BLIZZARD_RA1 \
		-Dgcc \
		-DSIM_MODE=0

	CFLAGS = \
		$(FPU_FLAGS) \
		$(DEFINES) \
		-mthumb \
		$(CPU_FLAGS) \
		-ffunction-sections \
		-fdata-sections \
		-MD \
		-std=c99 \
		-Wall \
		-pedantic \
		-O0 \
		-g3 \
		-Wno-unused-but-set-variable \
		-include stdint.h \
		-w

	LDFLAGS = \
		$(FPU_FLAGS) \
		$(DEFINES) \
		-fno-builtin-printf \
		-mthumb \
		$(CPU_FLAGS) \
		-Wl,--gc-sections \
		-T gcc-files/tm4c123gh6pm.ld.exception

	INCLUDES = -Ilib/std/nanoprintf

	# Source files
	SRC_FILES = \
		gcc-files/startup_gcc.c \
		FreakyRTOS/OS.c \
		FreakyRTOS/osasm_gcc.s \
		FreakyRTOS/OS_Lists.c \
		FreakyRTOS/OS_LaunchPad.c \
		FreakyRTOS/OS_Fifo.c \
		FreakyRTOS/OS_MailBox.c \
		FreakyRTOS/OS_Sema4.c \
		FreakyRTOS/ADC.c \
		FreakyRTOS/ST7735.c \
		FreakyRTOS/UART0int.c \
		FreakyRTOS/Interpreter.c \
		FreakyRTOS/FIFOsimple.c \
		FreakyRTOS/eDisk.c \
		FreakyRTOS/OS_CLI_Commands/cat.c \
		FreakyRTOS/OS_CLI_Commands/CLI_Commands.c \
		FreakyRTOS/OS_CLI_Commands/write.c \
		FreakyRTOS/OS_CLI_Commands/ls.c \
		FreakyRTOS/OS_CLI_Commands/touch.c \
		FreakyRTOS/OS_CLI_Commands/rm.c \
		FreakyRTOS/OS_CLI_Commands/pwd.c \
		FreakyRTOS/OS_CLI_Commands/cd.c \
		FreakyRTOS/OS_CLI_Commands/mount.c \
		FreakyRTOS/OS_CLI_Commands/umount.c \
		FreakyRTOS/OS_CLI_Commands/mkfs.c \
		FreakyRTOS/OS_CLI_Commands/mkdir.c \
		inc/Timer0A.c \
		inc/Timer1A.c \
		inc/Timer2A.c \
		inc/Timer3A.c \
		inc/Timer4A.c \
		inc/Timer5A.c \
		inc/PLL.c \
		inc/LaunchPad.c \
		wilc/bsp/source/nm_bsp_tm4c.c \
		wilc/bus_wrapper/source/nm_bus_wrapper_tm4c.c \
		wilc/common/source/nm_common.c \
		wilc/driver/source/m2m_wifi.c \
		wilc/driver/source/m2m_hif.c \
		wilc/driver/source/m2m_periph.c \
		wilc/driver/source/m2m_coex.c \
		wilc/driver/source/nmdrv.c \
		wilc/driver/source/nmdrv_crt.c \
		wilc/driver/source/nmasic.c \
		wilc/driver/source/nmbus.c \
		wilc/driver/source/nmi2c.c \
		wilc/driver/source/nmsdio.c \
		wilc/driver/source/nmspi.c \
		wilc/driver/source/nmuart.c \
		wilc/driver/source/spi_flash.c \
		src/main.c \
		lib/std/string_lite/string_lite.c \
		lib/std/nanoprintf/wrapper_nanoprintf.c \
		src/Networking.c
else
	BUILD_DIR = build/sw

	CC = gcc
	OBJCOPY = objcopy

	# Hardware mode
	CPU_FLAGS =

	FPU_FLAGS = 

	DEFINES = -DSIM_MODE=1

	CFLAGS = \
		$(FPU_FLAGS) \
		$(DEFINES) \
		$(CPU_FLAGS) \
		-std=c99 \
		-Wall \
		-pedantic \
		-Oz \
		-g3 \
		-Wno-unused-but-set-variable \
		-w

	LDFLAGS = 

	INCLUDES = -Ilib/std/string_lite \
			-Ilib/std/stdio_lite \
			-Ilib/std/nanoprintf \

	SRC_FILES = testingProtocol/stubs.c \
			testingProtocol/main.c
endif

CFLAGS += $(EXTRA_CFLAGS)

SRC_FILES += \
		src/ip.c \
		src/mac.c \
		src/Networking_Globs.c \
		src/UDP.c \
		src/internet_checksum.c \
		src/ICMP.c \
		src/DHCP.c \
		src/dhcp_client.c \
		src/ping.c \
		src/arp.c \
		src/userApp_udp.c

# Include paths
INCLUDES += $(sort $(foreach src_f,$(SRC_FILES),-I$(patsubst %/,%,$(dir $(src_f)))))

ifeq ($(MODE), hw)
	INCLUDES += -Iwilc/bsp/include
	INCLUDES += -Iwilc/bus_wrapper/include
	INCLUDES += -Iwilc/common/include
	INCLUDES += -Iwilc/driver/include
	INCLUDES += -Iwilc/wilc1000_module_config/tm4c
	INCLUDES += -Iwilc
	INCLUDES += -ItestingProtocol
endif

# Object files
OBJ_FILES = $(patsubst %.c,$(BUILD_DIR)/%.o,$(filter %.c,$(SRC_FILES))) \
			$(patsubst %.s,$(BUILD_DIR)/%.o,$(filter %.s,$(SRC_FILES)))

# Target executable
TARGET = $(BUILD_DIR)/exe

# Default target
all: $(TARGET).elf $(TARGET).bin

# Build the target executable
$(TARGET).elf: $(OBJ_FILES)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^ $(LDFLAGS)

# Compile C source files
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

# Compile assembly source files
$(BUILD_DIR)/%.o: %.s | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

# Convert ELF to BIN
$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

# Clean up
.PHONY: clean
clean:
	rm -rf build/*

# Flash the binary
.PHONY: flash
flash: $(TARGET).bin
	lm4flash build/hw/exe.bin

.PHONY: dump
dump: all
	@echo "Dumping object files (see build directory):"
	@for obj in $(OBJ_FILES); do \
		echo "  $$obj.dump"; \
		arm-none-eabi-objdump -d $$obj > $$obj.dump; \
	done
	@arm-none-eabi-objdump -d $(TARGET).elf > $(TARGET).elf.dump

# Include dependency files
-include $(OBJ_FILES:.o=.d)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

help:
	@echo "Makefile for TM4C123GH6PM project"
	@echo "Usage:"
	@echo "  make [BOARD=MotorBoard|SensorBoard]"
	@echo "  make clean"
	@echo "  make flash"
	@echo ""
	@echo "Useful flags:"
	@echo "  -B		Force build all files"
	@echo "  -j		Enable parallel build"
	@echo "Options:"
	@echo "  BOARD      Select the board (MotorBoard or SensorBoard)"
	@echo ""
	@echo "Targets:"
	@echo "  all        Build the project (default target)"
	@echo "  clean      Clean up build files"
	@echo "  flash      Flash the binary to the device"
	@echo ""
	@echo "Note:"
	@echo "  - The default board is MotorBoard if not specified."
	@echo "  - Make sure to have the required tools installed (arm-none-eabi-gcc, lm4flash)."
	@echo "  - The flash target depends on the project being built, so make flash will run make all first."



