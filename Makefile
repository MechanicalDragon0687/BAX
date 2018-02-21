ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM>")
endif

include $(DEVKITARM)/base_tools

COMMON  := "$(shell pwd)/common"
RELDIR  := "$(shell pwd)/release"
RELFILE := $(RELDIR).zip

export INCLUDE := -I$(COMMON)
export ARCH    := -marm -mno-thumb-interwork
export ASFLAGS := -x assembler-with-cpp -ggdb
export CFLAGS  := -O2 -std=c11 -pipe -fomit-frame-pointer \
                  -ffunction-sections -ffast-math -Wall -Wextra \
                  -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-main -ggdb
export LDFLAGS := -Tlink.ld -Wl,--gc-sections,--nmagic,-z,max-page-size=4 -nostartfiles -ggdb

ELF  := oldarm/oldarm.elf mpcore/mpcore.elf
FIRM := BAX.firm

.PHONY: all $(FIRM) clean release
all: $(FIRM)

clean:
	@$(foreach elf, $(ELF), \
		$(MAKE) --no-print-directory -C $$(dirname $(elf)) clean;)
	@rm -rf $(FIRM) $(RELDIR) $(RELFILE)

%.elf: .FORCE
	@echo "Building $@"
	@$(MAKE) --no-print-directory -C $(@D)

$(FIRM): $(ELF)
	firmtool build $@ -i -D $(ELF) -C NDMA XDMA

release: $(ELF) $(FIRM)
	@mkdir -p $(RELDIR)
	@cp -av $^ $(RELDIR)
	@cp -av README.md $(RELDIR)
	@zip -r $(RELFILE) $(RELDIR)

.FORCE:
