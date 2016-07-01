#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

include $(DEVKITARM)/ds_rules

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# DATA is a list of directories containing data files
# INCLUDES is a list of directories containing header files
# SPECS is the directory containing the important build and link files
#---------------------------------------------------------------------------------
TARGET 		:=  arm9loaderhax
BUILD		:=	build
SOURCES		:=	source source/fatfs source/fatfs/sdmmc
DATA		:=	data
INCLUDES	:=	source source/fatfs source/fatfs/sdmmc

NAME        :=  BootAnim9
LOADER      :=  loader
RELEASEDIR  :=  release

#---------------------------------------------------------------------------------
# Setup some defines
#---------------------------------------------------------------------------------

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
ARCH	 :=	-marm -march=armv5te -mtune=arm946e-s

CFLAGS   := $(ARCH) \
			-g -flto -Wall -O2 \
			-fomit-frame-pointer -ffast-math \
			-std=c99

CFLAGS	 += $(INCLUDE) -DARM9
CXXFLAGS := $(CFLAGS) -fno-rtti -fno-exceptions

LDFLAGS  := -nostartfiles -T../linker.ld -g $(ARCH) -Wl,-Map,$(notdir $*.map)
ASFLAGS	 :=	-g $(ARCH)

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:=

#---------------------------------------------------------------------------------
# any extra libraries we wish to link with the project (order is important)
#---------------------------------------------------------------------------------
LIBS    :=


#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export OUTPUT	:=	$(CURDIR)/$(TARGET)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
			$(foreach dir,$(DATA),$(CURDIR)/$(dir))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
#---------------------------------------------------------------------------------
	export LD	:=	$(CC)
#---------------------------------------------------------------------------------
else
#---------------------------------------------------------------------------------
	export LD	:=	$(CXX)
#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------

export OFILES	:= $(addsuffix .o,$(BINFILES)) \
			$(SFILES:.s=.o) $(CPPFILES:.cpp=.o) $(CFILES:.c=.o)

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
			$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
			-I$(CURDIR)/$(BUILD)

export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)

.PHONY: $(BUILD) all loader release clean

#---------------------------------------------------------------------------------
all: $(BUILD) loader

$(BUILD):
	@[ -d $(OUTPUT_D) ] || mkdir -p $(OUTPUT_D)
	@[ -d $(BUILD) ] || mkdir -p $(BUILD)
	@make --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

loader:
	@make --no-print-directory -C $(CURDIR)/$(LOADER) -f $(CURDIR)/$(LOADER)/Makefile

release: $(BUILD) loader
	@mkdir -p $(CURDIR)/$(RELEASEDIR)/anim
	@cp -a $(CURDIR)/$(TARGET).bin $(CURDIR)/$(RELEASEDIR)/$(TARGET).bin
	@cp -a $(CURDIR)/$(LOADER)/$(LOADER).bin $(CURDIR)/$(RELEASEDIR)/anim/$(LOADER).bin
	@cp -a  $(DATA)/calibrator $(CURDIR)/$(RELEASEDIR)/anim/calibrator
	@echo Done!

#---------------------------------------------------------------------------------
clean:
	@make --no-print-directory -C $(CURDIR)/$(LOADER) -f $(CURDIR)/$(LOADER)/Makefile clean
	@rm -rf $(BUILD) $(OUTPUT).bin $(CURDIR)/$(RELEASEDIR)
	@echo cleaned $(NAME)

#---------------------------------------------------------------------------------
else

DEPENDS	:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
$(OUTPUT).bin	:	$(OUTPUT).elf
$(OUTPUT).elf	:	$(OFILES)

#---------------------------------------------------------------------------------
%.bin: %.elf
	@$(OBJCOPY) --set-section-flags .bss=alloc,load,contents -O binary $< $@
	@echo built $(NAME)
	@rm -f $(OUTPUT).elf

-include $(DEPENDS)


#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------
