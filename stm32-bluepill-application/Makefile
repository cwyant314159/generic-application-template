# The build tools used in the targets.
TOOL_PREFIX := arm-none-eabi-
CC      := $(TOOL_PREFIX)gcc -c -xc
CXX     := $(TOOL_PREFIX)gcc -c -xc++
LD      := $(TOOL_PREFIX)gcc
AS      := $(TOOL_PREFIX)gcc -c -xassembler-with-cpp
AR      := $(TOOL_PREFIX)ar
OBJCOPY := $(TOOL_PREFIX)objcopy
OBJDUMP := $(TOOL_PREFIX)objdump
RM      := rm -rf
MKDIR   := mkdir -p

# The output binary directory and name.
BIN_DIR  := bin
BIN_NAME := blinky
ELF      := $(BIN_NAME).elf
HEX      := $(BIN_NAME).hex

# The application's linker script
LDSCRIPT := app/linker/STM32F103C8TX_FLASH.ld

# To switch to the version of the application
# using the STM32 HAL, set this variable to
# true on the command line when calling Make.
USE_HAL ?= 0

# Include directories and compiler
# include (-I) argument creation. For
# ease of addition, put each new
# include directory on its own line
# with the += assignment.
INC_DIRS :=
INC_DIRS += app/include
INC_DIRS += dependencies/STM32CubeF1/Drivers/CMSIS/Include
INC_DIRS += dependencies/STM32CubeF1/Drivers/CMSIS/Device/ST/STM32F1xx/Include

ifeq ($(USE_HAL), 1)
INC_DIRS += dependencies/STM32CubeF1/Drivers/STM32F1xx_HAL_Driver/Inc
INC_DIRS += dependencies/STM32CubeF1/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy
endif

INC_FLAGS := $(foreach dir, $(INC_DIRS), $(addprefix -I, $(dir)))

# Source directory list and source file
# list generation. For ease of addition,
# put each new source directory on its
# own line. The SRC_FILE variable is
# created by recursively searching
# these directories for *.c and *.cpp
# files.
SRC_DIRS :=
SRC_DIRS += app/src

ifeq ($(USE_HAL), 1)
SRC_DIRS += dependencies/STM32CubeF1/Drivers/STM32F1xx_HAL_Driver/Src
endif

SRC_FILES :=
SRC_FILES += $(foreach dir, $(SRC_DIRS), $(shell find $(dir) -type f -name '*.cpp'))
SRC_FILES += $(foreach dir, $(SRC_DIRS), $(shell find $(dir) -type f -name '*.c'))
SRC_FILES += $(foreach dir, $(SRC_DIRS), $(shell find $(dir) -type f -name '*.s'))

# The object directory and object file
# list generation. These shouldn't
# really change. Object files under the
# $(OBJ_DIR) will mirror the path of
# source files under the $(SRC_DIRS).
OBJ_DIR  := obj

OBJS := $(SRC_FILES:=.o)
OBJS := $(foreach obj, $(OBJS), $(addprefix $(OBJ_DIR)/, $(obj)))

# Common flags are passed to both the C and C++ compiler as well as the
# assembler.
COMMON_FLAGS :=
COMMON_FLAGS += -mcpu=cortex-m3
COMMON_FLAGS += -MMD
COMMON_FLAGS += -MP
COMMON_FLAGS += -g
COMMON_FLAGS += -mthumb
COMMON_FLAGS += -mfloat-abi=soft

# Warning flags are applied to both
# CFLAGS and CXXFLAGS through the
# COMFLAGS variable. If a certain
# warning only applies to one type of
# compiler, it must be placed in the
# compiler specific flags variable
WARN_FLAGS :=
WARN_FLAGS += -Wall
WARN_FLAGS += -Wextra
WARN_FLAGS += -Wundef
WARN_FLAGS += -Wshadow
WARN_FLAGS += -Wredundant-decls
WARN_FLAGS += -Wno-unused-parameter

# Common compiler flags are passed to
# both the CC and CXX compilers.
COMPILE_FLAGS := $(COMMON_FLAGS)
COMPILE_FLAGS += $(WARN_FLAGS)
COMPILE_FLAGS += -fstack-usage
COMPILE_FLAGS += -ffunction-sections
COMPILE_FLAGS += -fdata-sections
COMPILE_FLAGS += -fno-common
COMPILE_FLAGS += -DSTM32F103xB
COMPILE_FLAGS += -O0
COMPILE_FLAGS += -MMD
COMPILE_FLAGS += -MP
COMPILE_FLAGS += $(INC_FLAGS)

ifeq ($(USE_HAL), 1)
COMPILE_FLAGS += -DUSE_HAL_DRIVER
endif

# CFLAGS are C compiler specific flags.
# These flags are NOT passed to CXX
CFLAGS := $(COMPILE_FLAGS)
CFLAGS += -Wimplicit-function-declaration
CFLAGS += -Wmissing-prototypes
CFLAGS += -Wstrict-prototypes
CFLAGS += -std=c99

# CXXFLAGS are C++ compiler specifi
# flags. These flags are not passed to
# CC.
CXXFLAGS := $(COMPILE_FLAGS)
CXXFLAGS += -Weffc++
CXXFLAGS += -fno-exceptions
CXXFLAGS += -fno-rtti
CXXFLAGS += -fno-use-cxa-atexit
CXXFLAGS += -std=c++11

# ASFLAGS are the assembler specific
# flags.
ASFLAGS := $(COMMON_FLAGS) 

# Linker flags are passed to the
# compiler only during the link step.
# Neither the CFLAGS nor CXXFLAGS are
# passed to the linker. If shared
# options are required, they must be
# explicitly listed.
LDFLAGS := -mcpu=cortex-m3
LDFLAGS += -mthumb
LDFLAGS += -mfloat-abi=soft
LDFLAGS += -T$(LDSCRIPT)
LDFLAGS += -Wl,-Map="$(BIN_DIR)/$(BIN_NAME).map"
LDFLAGS += -static
LDFLAGS += --specs=nosys.specs
LDFLAGS += --specs=nano.specs
LDFLAGS += -Wl,--gc-sections
LDFLAGS += -Wl,--no-warn-rwx-segments
LDFLAGS += -Wl,--start-group
LDFLAGS += -lm
LDFLAGS += -lstdc++
LDFLAGS += -Wl,--end-group

# This variable is used by the dump
# target to format the output of Make
# variables. Notice how it uses the
# regular equal "=" instead of the
# colon equal ":=". This is done so
# that variable expansion is defered
# until this variable is used.
DUMP_FMT := tr " " "\n" | sed 's/^\(\w\|-\)/    \1/; /^$$/d'

# This is the default target (commonly
# called all). It in turn triggers the
# final binary linker target.
#
all: $(BIN_DIR)/$(ELF)

# This is the linker target that uses
# all of the compiled OBJS files and
# LDFLAGS to create the final binary.
#
$(BIN_DIR)/$(ELF): $(OBJS)
	@$(MKDIR) $$(dirname $@)
	$(LD) $(LDFLAGS) $^ -o $@

$(OBJ_DIR)/%.s.o: %.s
	@$(MKDIR) $$(dirname $@)
	$(AS) $(ASFLAGS) -o $@ $<

# This is the target that compiles all
# C files into object files under the
# $(OBJ_DIR).
$(OBJ_DIR)/%.c.o: %.c
	@$(MKDIR) $$(dirname $@)
	$(CC) $(CFLAGS) -o $@ $<

# This is the target that compiles all
# C++ files into object files under the
# $(OBJ_DIR).
$(OBJ_DIR)/%.cpp.o: %.cpp
	@$(MKDIR) $$(dirname $@)
	$(CXX) $(CXXFLAGS) -o $@ $<

# This target downloads the elf file to
# board using the helper script.
.PHONY: flash
flash: $(BIN_DIR)/$(ELF)
	@./scripts/flash.sh $<

# This target removes all build files
# and the final binary.
.PHONY: clean
clean:
	$(RM) $(OBJ_DIR)
	$(RM) $(BIN_DIR)

# This target is mostly used for
# debugging. It pretty prints the
# contents of Make variables.
.PHONY: dump
dump:
	@echo "CC    : $(CC)"
	@echo "CXX   : $(CXX)"
	@echo "LD    : $(LD)"
	@echo "AR    : $(AR)"
	@echo "RM    : $(RM)"
	@echo "MKDIR : $(MKDIR)"
	@echo "BIN   : $(BIN_DIR)/$(BIN)"
	@echo ""
	@echo "INC_DIRS:"
	@echo "$$(echo "$(INC_DIRS)" | $(DUMP_FMT))"
	@echo ""
	@echo "SRC_DIRS:"
	@echo "$$(echo "$(SRC_DIRS)" | $(DUMP_FMT))"
	@echo ""
	@echo "SRC_FILES:"
	@echo "$$(echo "$(SRC_FILES)" | $(DUMP_FMT))"
	@echo ""
	@echo "OBJS:"
	@echo "$$(echo "$(OBJS)" | $(DUMP_FMT))"
	@echo ""
	@echo "CFLAGS:"
	@echo "$$(echo "$(CFLAGS)" | $(DUMP_FMT))"
	@echo ""
	@echo "CXXFLAGS:"
	@echo "$$(echo "$(CXXFLAGS)" | $(DUMP_FMT))"
	@echo ""
	@echo "LDFLAGS"
	@echo "$$(echo "$(LDFLAGS)" | $(DUMP_FMT))"

# Not quite sure how they work but .d
# (a.k.a dep files) allow rebuilds to
# happen if header files changes.
DEPS := $(shell find $(OBJ_DIR) -name '*.d')
include $(DEPS)
