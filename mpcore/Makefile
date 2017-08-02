TARGET  := $(notdir $(CURDIR))

OBJS := src/boot.s.o src/main.c.o src/gic.c.o

ARCH    += -march=armv6k -mtune=mpcore -DARM11

ASFLAGS := $(ARCH) -x assembler-with-cpp $(INCLUDE)
CFLAGS  := $(ARCH) -O2 -std=c99 -pipe -Wextra -fomit-frame-pointer \
           -ffunction-sections -ffast-math $(INCLUDE) -mno-unaligned-access
LDFLAGS := -Tlink.ld -Wl,--gc-sections,-z,max-page-size=4096 -nostartfiles

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
