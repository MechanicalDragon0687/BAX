TARGET  := makebax

CFLAGS  ?= -O2
LDFLAGS := -lpthread -llz4 -lvpx

ifeq ($(OS),Windows_NT)
	CFLAGS  := $(CFLAGS) -mno-ms-bitfields
	LDFLAGS := -static $(LDFLAGS)
	TARGET  := $(TARGET).exe
endif

OBJDIR  := build
SRCDIR  := source
_OBJS   := bax.o main.o ringbuf.o baxbuild.o lzcompress.o postprocess.o video.o vpx.o
OBJS    := $(patsubst %,$(OBJDIR)/%,$(_OBJS))

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p "$(@D)"
	$(CC) -c -o $@ $^ $(CFLAGS)

.PHONY: clean
clean:
	@rm -rf $(OBJDIR) $(TARGET)
