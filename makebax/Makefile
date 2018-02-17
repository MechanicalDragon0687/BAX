TARGET  := makebax

CXXFLAGS := -std=c++1z -O2 -Wall -Wextra -fopenmp
LDFLAGS  := -lpthread -lgomp -llz4 -lvpx

ifeq ($(OS),Windows_NT)
	CFLAGS  := $(CFLAGS) -mno-ms-bitfields
	LDFLAGS := -static $(LDFLAGS) # all explicit libs should be MIT compatible
	TARGET  := $(TARGET).exe
endif

OBJDIR  := build
SRCDIR  := source
_OBJS   := bax.o main.o ivf.o
OBJS    := $(patsubst %,$(OBJDIR)/%,$(_OBJS))

$(TARGET): $(OBJS)
	$(CXX) $^ $(LDFLAGS) -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cc
	@mkdir -p "$(@D)"
	$(CXX) $^ $(CXXFLAGS) -c -o $@

.PHONY: clean
clean:
	@rm -rf $(OBJDIR) $(TARGET)
