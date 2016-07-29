# Makefile is originally from CakesFW by b1l1s / mid-kid
# Slightly edited it to fit my purposes

rwildcard = $(foreach d, $(wildcard $1*), $(filter $(subst *, %, $2), $d) $(call rwildcard, $d/, $2))

CC := arm-none-eabi-gcc
AS := arm-none-eabi-as
LD := arm-none-eabi-ld
OC := arm-none-eabi-objcopy

dir_source := source
dir_build  := build
dir_out    := release
dir_loader := loader

ASFLAGS  := -mlittle-endian -march=armv5te -mcpu=arm946e-s

CFLAGS   := $(ASFLAGS) -g -O2 -flto \
			-marm -Wall -Wextra \
			-fomit-frame-pointer -fshort-wchar \
			-Wpedantic -pedantic \
			-Wcast-align -Wmissing-include-dirs \
			-Wredundant-decls -Wshadow \
			-Wno-unused -Wno-main -ffast-math \
			$(INCLUDE)

LDFLAGS  := -nostartfiles -T linker.ld -g $(ASFLAGS)

objects = $(patsubst $(dir_source)/%.s, $(dir_build)/%.o, \
		  $(patsubst $(dir_source)/%.c, $(dir_build)/%.o, \
		  $(call rwildcard, $(dir_source), *.s *.c)))

.PHONY: all
all: arm9loaderhax.bin

.PHONY: clean
clean:
	@rm -rf release $(dir_build) $(dir_out) arm9loaderhax.bin
	@echo cleaned BootAnim9

release: arm9loaderhax.bin
	@mkdir -p release/anim
	@cp -a arm9loaderhax.bin release/arm9loaderhax.bin
	@cp -a data/calibrator release/anim/calibrator
	@echo Release done!

arm9loaderhax.bin: $(dir_build)/main.elf
	$(OC) -S -O binary $< $@
	@echo built BootAnim9

$(dir_build)/main.elf: $(objects)
	$(CC) $^ $(LDFLAGS) $(OUTPUT_OPTION)

# Optimization flags for certain files
$(dir_build)/anim.o: CFLAGS += -O3
$(dir_build)/draw.o: CFLAGS += -O3
$(dir_build)/quicklz.o: CFLAGS += -O3

$(dir_build)/%.o: $(dir_source)/%.c
	@mkdir -p "$(@D)"
	$(COMPILE.c) $(CFLAGS) $(OUTPUT_OPTION) $<

$(dir_build)/%.o: $(dir_source)/%.s
	@mkdir -p "$(@D)"
	$(COMPILE.s) $(ASFLAGS) $(OUTPUT_OPTION) $<

include $(call rwildcard, $(dir_build), *.d)
