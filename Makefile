ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
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
export LDFLAGS := -Tlink -Wl,--gc-sections,-z,max-page-size=512 -nostartfiles -ggdb

ELF  := oldarm/oldarm.elf mpcore/mpcore.elf
FIRM := BAX.firm

.PHONY: all firm elf clean
all: firm

clean:
	@set -e; for elf in $(ELF); do \
		$(MAKE) --no-print-directory -C $$(dirname $$elf) clean; \
	done
	@rm -rf $(FIRM) $(OUTDIR)

elf:
	@set -e; for elf in $(ELF); do \
		echo "Building $$elf"; \
		$(MAKE) --no-print-directory -C $$(dirname $$elf); \
	done

firm: elf
	firmtool build $(FIRM) -i -D $(ELF) -C NDMA XDMA
