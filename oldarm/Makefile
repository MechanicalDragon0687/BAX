TARGET  := $(shell basename $(CURDIR))

SUBARCH := -DARM9 -mcpu=arm946e-s -mfloat-abi=soft $(ARCH)
SOURCE  := src
BUILD   := bin

INCLUDE += -Isrc
ASFLAGS += $(SUBARCH) $(INCLUDE)
CFLAGS  += $(SUBARCH) $(INCLUDE)
LDFLAGS += $(SUBARCH)

SOURCE_OUTPUT := $(patsubst $(SOURCE)/%.c, $(BUILD)/%.c.o, \
                 $(patsubst $(SOURCE)/%.s, $(BUILD)/%.s.o, \
                 $(shell find "$(SOURCE)" -name '*.c' -o -name '*.s')))

.PHONY: all
all: $(TARGET).elf

.PHONY: clean
clean:
	@rm -rf $(BUILD) $(TARGET).elf

$(TARGET).elf: $(SOURCE_OUTPUT)
	@mkdir -p "$(@D)"
	$(CC) $(LDFLAGS) $^ -o $@

$(BUILD)/%.c.o: $(SOURCE)/%.c
	@mkdir -p "$(@D)"
	$(CC) -c $(CFLAGS) -o $@ $<

$(BUILD)/%.s.o: $(SOURCE)/%.s
	@mkdir -p "$(@D)"
	$(CC) -c $(ASFLAGS) -o $@ $<
