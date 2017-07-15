TARGET  := $(notdir $(CURDIR))

TRIPLET := arm-none-eabi
AS := $(TRIPLET)-as
CC := $(TRIPLET)-gcc
LD := $(TRIPLET)-ld
OC := $(TRIPLET)-objcopy

OBJS := src/boot.o src/main.o
CFLAGS  := -Os -ggdb -std=c99 -Wextra -march=armv6k -mtune=mpcore -mno-thumb-interwork -marm
LDFLAGS := -Tlink.ld -nostdlib

.PHONY: all clean

all: $(TARGET).o

clean:
	rm -rf $(OBJS) $(TARGET).*

$(TARGET).elf: $(OBJS)
	$(LD) $(OBJS) $(LDFLAGS) -o $@

$(TARGET).bin: $(TARGET).elf
	$(OC) -O binary $< $@

$(TARGET).o: $(TARGET).bin
	$(OC) -I binary -O elf32-littlearm --binary-architecture armv5te $< $@

%.o: %.c %.s
	$(CC) $(CFLAGS) $% -c -o $@
