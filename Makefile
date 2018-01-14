ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM>")
endif

include $(DEVKITARM)/base_tools

export OUTDIR  := "$(shell pwd)/output"
export COMMON  := "$(shell pwd)/common"

export INCLUDE := -I$(COMMON)
export ARCH    := -marm -mno-thumb-interwork
export ASFLAGS := -x assembler-with-cpp -ggdb
export CFLAGS  := -O2 -std=c99 -pipe -fomit-frame-pointer \
                  -ffunction-sections -ffast-math -Wall -Wextra\
                  -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-main -ggdb
export LDFLAGS := -Tlink -Wl,--gc-sections,--nmagic,-z,max-page-size=4 -nostartfiles -ggdb

ELF  := oldarm/oldarm.elf mpcore/mpcore.elf
FIRM := BAX.firm

.PHONY: all firm clean
all: firm

clean:
	@$(foreach elf, $(ELF), \
		$(MAKE) --no-print-directory -C $$(dirname $(elf)) clean;)
	@rm -rf $(FIRM) $(OUTDIR)

%.elf: .FORCE
	@echo "Building $@"
	@$(MAKE) --no-print-directory -C $(@D)

firm: $(ELF)
	firmtool build $(FIRM) -i -D $(ELF) -C NDMA XDMA

.FORCE:
