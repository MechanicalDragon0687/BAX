ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

TRIPLET := arm-none-eabi
export AS = $(TRIPLET)-as
export CC = $(TRIPLET)-gcc
export LD = $(TRIPLET)-ld
export OC = $(TRIPLET)-objcopy

export INCLUDE := -I"$(shell pwd)/common" -Isrc
export ARCH := -marm -mno-thumb-interwork $(INCLUDE)

OLDARM  := oldarm/oldarm.elf
MPCORE  := mpcore/mpcore.elf
FIRM    := BAX.firm

OLDARM_DIR := $(shell dirname "$(OLDARM)")
MPCORE_DIR := $(shell dirname "$(MPCORE)")

.PHONY: all firm clean
all: $(OLDARM) $(MPCORE) firm

clean:
	@$(MAKE) --no-print-directory -C $(OLDARM_DIR) clean
	@$(MAKE) --no-print-directory -C $(MPCORE_DIR) clean
	@rm -rf $(FIRM)

$(OLDARM):
	@echo "\e[92mBuilding $(OLDARM)\e[0m"
	@$(MAKE) --no-print-directory -C $(OLDARM_DIR)

$(MPCORE):
	@echo "\e[92mBuilding $(MPCORE)\e[0m"
	@$(MAKE) --no-print-directory -C $(MPCORE_DIR)

firm: $(OLDARM) $(MPCORE)
	firmtool build $(FIRM) -D $^ -C NDMA XDMA
