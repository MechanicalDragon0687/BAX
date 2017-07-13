rwildcard = $(foreach d, $(wildcard $1*), $(filter $(subst *, %, $2), $d) $(call rwildcard, $d/, $2))

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

include $(DEVKITARM)/base_tools

TARGET  := $(notdir $(CURDIR))

SOURCE  := source
BUILD   := build

ARCH    := -mlittle-endian -march=armv5te -mcpu=arm946e-s

ASFLAGS := $(ARCH) -ggdb -x assembler-with-cpp -I$(SOURCE)

CFLAGS  := $(ARCH) -ggdb -O2 -marm \
			-fomit-frame-pointer -pipe \
			-Wall -Wextra -ffast-math \
			-I$(SOURCE) -std=gnu99 -ffunction-sections

LDFLAGS := $(ARCH) -nostartfiles -Wl,--gc-sections,-Map,$(BUILD)/$(TARGET).map -ffreestanding -T linker.ld

OBJECTS = $(patsubst $(SOURCE)/%.s, $(BUILD)/%.s.o, \
		  $(patsubst $(SOURCE)/%.c, $(BUILD)/%.c.o, \
		  $(call rwildcard, $(SOURCE), *.s *.c)))

.PHONY: all
all: $(BUILD)/$(TARGET).bin

.PHONY: clean
clean:
	@rm -rf $(BUILD) $(RELEASE) profile.svg

profile: $(BUILD)/$(TARGET).elf
	sh profile.sh $<

$(BUILD)/$(TARGET).bin: $(BUILD)/$(TARGET).elf
	$(OBJCOPY) -S -O binary $< $@

$(BUILD)/$(TARGET).elf: $(OBJECTS)
	$(CC) $^ $(LDFLAGS) -o $@

$(BUILD)/%.c.o: $(SOURCE)/%.c
	@mkdir -p "$(@D)"
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/%.s.o: $(SOURCE)/%.s
	@mkdir -p "$(@D)"
	$(CC) $(ASFLAGS) -c -o $@ $<

include $(call rwildcard, $(BUILD), *.d)
