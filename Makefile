rwildcard = $(foreach d, $(wildcard $1*), $(filter $(subst *, %, $2), $d) $(call rwildcard, $d/, $2))

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

include $(DEVKITARM)/base_tools

TARGET := $(notdir $(CURDIR))

SOURCE := source
BUILD  := build

IPC_DIR := ipc
IPC_O   := $(IPC_DIR)/$(IPC_DIR).o

ARCH   := -mlittle-endian -march=armv5te -mcpu=arm946e-s -nostartfiles

ASFLAGS := $(ARCH) -ggdb -x assembler-with-cpp -I$(SOURCE)
CFLAGS  := $(ARCH) -ggdb -O2 -fomit-frame-pointer -ffunction-sections \
           -marm -pipe -Wall -Wextra -ffast-math -I$(SOURCE) -std=gnu99

LDFLAGS := $(ARCH) -Wl,--gc-sections -ffreestanding -Tlink.ld

OBJECTS = $(patsubst $(SOURCE)/%.s, $(BUILD)/%.s.o, \
		  $(patsubst $(SOURCE)/%.c, $(BUILD)/%.c.o, \
		  $(call rwildcard, $(SOURCE), *.s *.c)))

.PHONY: all
all: $(BUILD)/$(TARGET).bin

.PHONY: clean
clean:
	@$(MAKE) -C $(IPC_DIR) clean
	@rm -rf $(BUILD)

$(BUILD)/$(TARGET).bin: $(BUILD)/$(TARGET).elf
	$(OBJCOPY) -S -O binary $< $@

$(IPC_O):
	$(MAKE) -C $(IPC_DIR)

$(BUILD)/$(TARGET).elf: $(OBJECTS) $(IPC_O)
	$(CC) $^ $(LDFLAGS) -o $@

$(BUILD)/%.c.o: $(SOURCE)/%.c
	@mkdir -p "$(@D)"
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/%.s.o: $(SOURCE)/%.s
	@mkdir -p "$(@D)"
	$(CC) $(ASFLAGS) -c -o $@ $<

include $(call rwildcard, $(BUILD), *.d)
