TARGET  := makebax

CXXFLAGS := -std=c++1z -O2 -Wall -fopenmp
LDFLAGS  := -lgomp -llz4 -lvpx

ifeq ($(OS),Windows_NT)
	CFLAGS  := $(CFLAGS) -mno-ms-bitfields
	LDFLAGS := -static $(LDFLAGS)
	TARGET  := $(TARGET).exe
endif

OBJDIR  := build
SRCDIR  := source
_OBJS   := bax.o main.o ivf.o
#lzcompress.o postprocess.o video.o vpx.o
OBJS    := $(patsubst %,$(OBJDIR)/%,$(_OBJS))

$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cc
	@mkdir -p "$(@D)"
	$(CXX) -c -o $@ $^ $(CXXFLAGS)

.PHONY: clean
clean:
	@rm -rf $(OBJDIR) $(TARGET)
