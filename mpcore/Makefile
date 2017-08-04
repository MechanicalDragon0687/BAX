TARGET  := $(notdir $(CURDIR))

OBJS := src/boot.s.o src/main.c.o src/gic.c.o src/gpu.c.o src/lz4.c.o src/vq.c.o

ARCH    += -march=armv6k -mtune=mpcore -DARM11

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
