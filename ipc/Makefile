TARGET  := $(notdir $(CURDIR))

TRIPLET := arm-none-eabi

AS := $(TRIPLET)-as
CC := $(TRIPLET)-gcc
LD := $(TRIPLET)-ld
OC := $(TRIPLET)-objcopy
OD := $(TRIPLET)-objdump

OBJS := source/boot.o source/main.o

CFLAGS  := -Os -ffreestanding -ggdb -std=c99 \
           -Wall -Wextra -march=armv6k -mtune=mpcore

LDFLAGS := -Tlink.ld -nostdlib

.PHONY: all clean

all: $(TARGET).h

clean:
	rm -rf $(OBJS) $(TARGET).h $(TARGET).bin $(TARGET).elf $(TARGET).asm

$(TARGET).h: $(TARGET).bin
	xxd -i $< > $@

$(TARGET).bin: $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $(TARGET).elf
	$(OC) -O binary $(TARGET).elf $(TARGET).bin
	$(OD) -D $(TARGET).elf > $(TARGET).asm

%.o: %.c %.s
	$(CC) $(CFLAGS) $% -c -o $@
