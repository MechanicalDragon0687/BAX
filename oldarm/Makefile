TARGET  := $(shell basename $(CURDIR))

# Hack, see savannah.gnu.org/bugs/?10593
COMMON  := ../common
SUBARCH := -DARM9 -mcpu=arm946e-s -mfloat-abi=soft $(ARCH)
SOURCE  := src
BUILD   := bin

INCLUDE += -I$(SOURCE)
ASFLAGS += $(SUBARCH) $(INCLUDE)
CFLAGS  += $(SUBARCH) $(INCLUDE)
LDFLAGS += $(SUBARCH)

SOURCE_OUTPUT := $(patsubst $(SOURCE)/%.c, $(BUILD)/%.c.o, \
                 $(patsubst $(SOURCE)/%.s, $(BUILD)/%.s.o, \
                 $(shell find "$(SOURCE)" -name '*.c' -o -name '*.s')))

COMMON_OUTPUT := $(patsubst $(COMMON)/%.c, $(BUILD)/%.common.c.o, \
                 $(patsubst $(COMMON)/%.s, $(BUILD)/%.common.s.o, \
                 $(shell find "$(COMMON)" -name '*.c' -o -name '*.s')))

.PHONY: all
all: $(TARGET).elf

.PHONY: clean
clean:
	@rm -rf $(BUILD) $(TARGET).elf

$(TARGET).elf: $(SOURCE_OUTPUT) $(COMMON_OUTPUT)
	@mkdir -p "$(@D)"
	$(CC) $(LDFLAGS) $^ -o $@


$(BUILD)/%.c.o: $(SOURCE)/%.c
	@mkdir -p "$(@D)"
	$(CC) -c $(CFLAGS) -o $@ $<

$(BUILD)/%.common.c.o: $(COMMON)/%.c
	@mkdir -p "$(@D)"
	$(CC) -c $(CFLAGS) -o $@ $<

$(BUILD)/%.s.o: $(SOURCE)/%.s
	@mkdir -p "$(@D)"
	$(CC) -c $(ASFLAGS) -o $@ $<

$(BUILD)/%.common.s.o: $(COMMON)/%.s
	@mkdir -p "$(@D)"
	$(CC) -c $(ASFLAGS) -o $@ $<
