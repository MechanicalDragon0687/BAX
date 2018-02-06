TARGET  := comp

CFLAGS  := -O2 -ffast-math -ggdb
LDFLAGS := -lpthread -llz4 -lavcodec -lavformat -lswscale -ggdb

OBJDIR  := build
SRCDIR  := source
_OBJS   := bax.o main.o ringbuf.o
OBJS    := $(patsubst %,$(OBJDIR)/%,$(_OBJS))

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p "$(@D)"
	$(CC) $(CFLAGS) $^ -c -o $@

.PHONY: clean
clean:
	@rm -rf $(OBJDIR) $(TARGET)
