rwildcard = $(foreach d, $(wildcard $1*), $(filter $(subst *, %, $2), $d) $(call rwildcard, $d/, $2))

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

include $(DEVKITARM)/base_tools

dir_source  := source
dir_build   := build

ARCH := -march=armv5te -mcpu=arm946e-s

ASFLAGS := $(ARCH) -gxcoff++

CFLAGS   := $(ARCH) -ggdb -ffreestanding -Os -flto \
			-mthumb -mno-thumb-interwork -fomit-frame-pointer \
			-Wall -Wextra -Wno-unused-parameter -ffast-math \
			$(ARCH) -mtune=arm946e-s -I$(dir_source) -ffunction-sections

LDFLAGS  := $(ARCH) -Wl,--gc-section,--use-blx,-Map,$(dir_build)/linker.map -nostartfiles -ffreestanding -T linker.ld

objects = $(patsubst $(dir_source)/%.s, $(dir_build)/%.s.o, \
		  $(patsubst $(dir_source)/%.c, $(dir_build)/%.c.o, \
		  $(call rwildcard, $(dir_source), *.s *.c)))

.PHONY: all
all: external $(dir_build)/main.bin

.PHONY: clean
clean:
	make -C ARM11 -f Makefile clean
	@rm -rf $(dir_build) $(dir_out)

external:
	make -C ARM11 -f Makefile

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
