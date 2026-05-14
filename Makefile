########################################
# Project
########################################

PROJECT = lora_sx127x_driver

########################################
# Directories
########################################

BUILD_DIR = build

SRC_DIR = src
INC_DIR = include
STARTUP_DIR = startup
LINKER_DIR = linker

########################################
# Toolchain
########################################

CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

########################################
# MCU
########################################

MCU = cortex-m3

########################################
# libopencm3
########################################

OPENCM3_DIR = ../libopencm3

########################################
# Flags
########################################

CFLAGS = \
  -mcpu=$(MCU) \
  -mthumb \
  -DSTM32F1 \
  -Wall \
  -Wextra \
  -Werror \
  -O2 \
  -ffunction-sections \
  -fdata-sections

LDFLAGS = \
  -T$(LINKER_DIR)/stm32f103c8.ld \
  -nostartfiles \
  -Wl,--gc-sections \
  -L$(OPENCM3_DIR)/lib \
  -lopencm3_stm32f1

########################################
# Includes
########################################

INCLUDES = \
  -I$(INC_DIR) \
  -I$(INC_DIR)/app \
  -I$(INC_DIR)/bsp \
  -I$(INC_DIR)/drivers/spi \
  -I$(INC_DIR)/drivers/sx127x \
  -I$(INC_DIR)/drivers/lora \
  -I$(INC_DIR)/protocol \
  -I$(OPENCM3_DIR)/include

########################################
# Sources
########################################

C_SOURCES = $(shell find $(SRC_DIR) -name "*.c")
ASM_SOURCES = $(STARTUP_DIR)/startup_stm32f103c8.s

########################################
# Objects
########################################

OBJECTS = $(C_SOURCES:.c=.o)
OBJECTS += $(ASM_SOURCES:.s=.o)

########################################
# Build
########################################

all: $(BUILD_DIR)/$(PROJECT).elf

$(BUILD_DIR)/$(PROJECT).elf: $(OBJECTS)
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(OBJECTS) $(LDFLAGS) -o $@
	$(SIZE) $@

########################################
# Compilation rules
########################################

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

%.o: %.s
	$(CC) $(CFLAGS) -c $< -o $@

########################################
# Output formats
########################################

bin: all
	$(OBJCOPY) -O binary $(BUILD_DIR)/$(PROJECT).elf $(BUILD_DIR)/$(PROJECT).bin

hex: all
	$(OBJCOPY) -O ihex $(BUILD_DIR)/$(PROJECT).elf $(BUILD_DIR)/$(PROJECT).hex

########################################
# Flash
########################################

flash: bin
	st-flash write $(BUILD_DIR)/$(PROJECT).bin 0x08000000

########################################
# Erase
########################################

erase:
	st-flash erase

reflash: erase flash

########################################
# Clean
########################################

clean:
	rm -rf $(BUILD_DIR)
	find . -name "*.o" -delete

########################################
# Node Targets
########################################

tx:
	$(MAKE) clean
	$(MAKE) APP_NODE=TX all

rx:
	$(MAKE) clean
	$(MAKE) APP_NODE=RX all

flash-tx:
	$(MAKE) clean
	$(MAKE) APP_NODE=TX flash

flash-rx:
	$(MAKE) clean
	$(MAKE) APP_NODE=RX flash

reflash-tx:
	$(MAKE) APP_NODE=TX erase
	$(MAKE) APP_NODE=TX flash

reflash-rx:
	$(MAKE) APP_NODE=RX erase
	$(MAKE) APP_NODE=RX flash

########################################
# Extra CFLAGS
########################################

CFLAGS += -DAPP_NODE_$(APP_NODE)

########################################
# Phony
########################################

.PHONY: \
	all \
	bin \
	hex \
	flash \
	erase \
	reflash \
	clean \
	tx \
	rx \
	flash-tx \
	flash-rx \
	reflash-tx \
	reflash-rx
