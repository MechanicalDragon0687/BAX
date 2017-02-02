rwildcard = $(foreach d, $(wildcard $1*), $(filter $(subst *, %, $2), $d) $(call rwildcard, $d/, $2))

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

include $(DEVKITARM)/base_tools

dir_source  := source
dir_build   := build

ARCH := -march=armv6k -mcpu=mpcore -mfloat-abi=hard

ASFLAGS := $(ARCH)

CFLAGS   := $(ASFLAGS) -g -ffreestanding -Os -flto \
			-marm -fomit-frame-pointer \
			-Wall -Wextra -Wno-main -ffast-math \
			$(ARCH) -mtune=mpcore -I$(dir_source) \
			-Wl,--gc-section,--use-blx -ffunction-sections \
			-mtp=soft

LDFLAGS  := -nostartfiles -nostdlib -ffreestanding -T linker.ld -g $(ASFLAGS)

objects = $(patsubst $(dir_source)/%.s, $(dir_build)/%.s.o, \
		  $(patsubst $(dir_source)/%.c, $(dir_build)/%.c.o, \
		  $(call rwildcard, $(dir_source), *.s *.c)))

.PHONY: all
all: $(dir_build)/main.bin

.PHONY: clean
clean:
	@rm -rf $(dir_build) $(dir_out)

$(dir_build)/main.bin: $(dir_build)/main.elf
	$(OBJCOPY) -S -O binary $< $@

$(dir_build)/main.elf: $(objects)
	$(CC) $^ $(LDFLAGS) $(OUTPUT_OPTION)

$(dir_build)/%.c.o: $(dir_source)/%.c
	@mkdir -p "$(@D)"
	$(COMPILE.c) $(OUTPUT_OPTION) $<

$(dir_build)/%.s.o: $(dir_source)/%.s
	@mkdir -p "$(@D)"
	$(COMPILE.s) $(OUTPUT_OPTION) $<

include $(call rwildcard, $(dir_build), *.d)
