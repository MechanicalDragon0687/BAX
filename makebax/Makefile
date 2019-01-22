TARGET  := makebax

CXXFLAGS := -std=c++1z -O2 -Wall -Wextra -fopenmp
LDFLAGS  := -lpthread -lgomp -llz4 -lopencv_core -lopencv_videoio

ifeq ($(OS),Windows_NT)
	CFLAGS  := $(CFLAGS) -mno-ms-bitfields
	LDFLAGS := -static $(LDFLAGS) # all explicit libs should be MIT compatible
	TARGET  := $(TARGET).exe
endif

OBJDIR  := bin
SRCDIR  := src
_OBJS   := bax.o compress.o main.o raw.o
OBJS    := $(patsubst %,$(OBJDIR)/%,$(_OBJS))

$(TARGET): $(OBJS)
	$(CXX) $^ $(LDFLAGS) -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cc
	@mkdir -p "$(@D)"
	$(CXX) $^ $(CXXFLAGS) -c -o $@

.PHONY: clean
clean:
	@rm -rf $(OBJDIR) $(TARGET)
