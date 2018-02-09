TARGET  := makebax

CFLAGS  := -O2
LDFLAGS := -lpthread -llz4

OBJDIR  := build
SRCDIR  := source
_OBJS   := bax.o main.o ringbuf.o baxbuild.o lzcompress.o postprocess.o video.o
OBJS    := $(patsubst %,$(OBJDIR)/%,$(_OBJS))

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p "$(@D)"
	$(CC) $(CFLAGS) $^ -c -o $@

.PHONY: clean
clean:
	@rm -rf $(OBJDIR) $(TARGET)
