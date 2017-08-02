TARGET  := $(notdir $(CURDIR))

OBJS := src/boot.s.o src/main.c.o src/arm/irq.c.o src/fs/ff/diskio.c.o \
        src/fs/ff/ff.c.o src/fs/ff/option/ccsbcs.c.o src/hw/sdmmc.c.o \
        src/arm/bootfirm.s.o

ARCH    += -march=armv5te -mtune=arm946e-s -DARM9

ASFLAGS := $(ARCH) -x assembler-with-cpp $(INCLUDE)
CFLAGS  := $(ARCH) -O2 -std=c99 -pipe -Wextra -fomit-frame-pointer \
           -ffunction-sections -ffast-math $(INCLUDE)
LDFLAGS := -Tlink.ld -Wl,--gc-sections,-z,max-page-size=4096 -nostartfiles

.PHONY: all clean

all: $(TARGET).elf

clean:
	@rm -rf $(OBJS) $(TARGET).*

$(TARGET).elf: $(OBJS)
	$(LINK.o) $^ -o $@

%.c.o: %.c
	$(CC) $(CFLAGS) $^ -c -o $@

%.s.o: %.s
	$(CC) $(ASFLAGS) $^ -c -o $@
