TARGET  := $(notdir $(CURDIR))

OBJS := src/boot.s.o src/main.c.o src/arm/irq.c.o src/ff/diskio.c.o \
        src/ff/ff.c.o src/ff/option/ccsbcs.c.o src/hw/sdmmc.c.o \
        src/arm/bootfirm.s.o src/hw/sha.c.o src/firm.c.o

ARCH    += -march=armv5te -mtune=arm946e-s -DARM9

INCLUDE += -Isrc
ASFLAGS += $(ARCH) -x assembler-with-cpp $(INCLUDE)
CFLAGS  += $(ARCH) $(INCLUDE)

.PHONY: all clean

all: $(TARGET).elf

clean:
	@rm -rf $(OBJS) $(TARGET).*

$(TARGET).elf: $(OBJS)
	$(CC) $^ $(LDFLAGS) -o $@

%.c.o: %.c
	$(CC) $(CFLAGS) $^ -c -o $@

%.s.o: %.s
	$(CC) $(ASFLAGS) $^ -c -o $@
