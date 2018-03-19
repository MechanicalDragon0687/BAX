PROCESSOR := ARM11

TARGET  := $(shell basename $(CURDIR))

# Hack, see savannah.gnu.org/bugs/?10593
COMMON  := ../common
SUBARCH := -D$(PROCESSOR) -mcpu=mpcore -mfloat-abi=hard -mfpu=vfpv2 $(ARCH)
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

include ../buildrules.mk
