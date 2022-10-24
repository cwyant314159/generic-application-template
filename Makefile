CC    := gcc -c -xc
CXX   := gcc -c -xc++
LD    := gcc
AR    := ar
RM    := rm -rf
MKDIR := mkdir -p

# The output binary directory and name.
BIN_DIR := bin
BIN     := template_program

# Include directories and compiler
# include (-I) argument creation. For
# ease of addition, put each new
# include directory on its own line
# with the += assignment.
INC_DIRS :=
INC_DIRS += include
INC_VARS := $(foreach dir, $(INC_DIRS), $(addprefix -I, $(dir)))

# Source directory list and source file
# list generation. For ease of addition,
# put each new source directory on its
# own line. The SRC_FILE variable is
# created by recursively searching
# these directories for *.c and *.cpp
# files.
SRC_DIRS :=
SRC_DIRS += src

SRC_FILES :=
SRC_FILES += $(foreach dir, $(SRC_DIRS), $(shell find $(dir) -type f -name '*.cpp'))
SRC_FILES += $(foreach dir, $(SRC_DIRS), $(shell find $(dir) -type f -name '*.c'))

# The object directory and object file
# list generation. These shouldn't
# really change. Object files under the
# $(OBJ_DIR) will mirror the path of
# source files under the $(SRC_DIRS).
# This does present the limitation that
# a C file cannot have the same name and
# be in the same location as a C++ file.
# Files with the same name but different
# file extensions must be placed in their
# own directory.
OBJ_DIR  := obj
OBJS     := $(filter %.o, $(SRC_FILES:.c=.o) $(SRC_FILES:.cpp=.o))
OBJS     := $(foreach obj, $(OBJS), $(addprefix $(OBJ_DIR)/, $(obj)))

# Warning flags are applied to both
# CFLAGS and CXXFLAGS through the
# COMFLAGS variable. If a certain
# warning only applies to one type of
# compiler, it must be placed in the
# compiler specific flags variable
WARNFLAGS :=
WARNFLAGS += -Wall
WARNFLAGS += -Wextra
WARNFLAGS += -Wshadow

# Common compiler flags are passed to
# both the CC and CXX compilers.
COMFLAGS := $(WARNFLAGS)
COMFLAGS += -ffunction-sections
COMFLAGS += -fdata-sections
COMFLAGS += -MMD
COMFLAGS += -MP
COMFLAGS += $(INC_VARS)

# CFLAGS are C compiler specific flags.
# These flags are NOT passed to CXX
CFLAGS := $(COMFLAGS)
CFLAGS += -std=c99

# CXXFLAGS are C++ compiler specific
# flags. These flags are not passed to
# CC.
CXXFLAGS := $(COMFLAGS)
CXXFLAGS += -std=c++11

# Linker flags are passed to the
# compiler only during the link step.
# Neither the CFLAGS nor CXXFLAGS are
# passed to the linker. If shared
# options are required, they must be
# explicitly listed.
LDFLAGS := -Wl,--gc-sections
LDFLAGS += -lm
LDFLAGS += -lstdc++ # commment at the end of a line

# This variable is used by the dump
# target to format the output of Make
# variables. Notice how it uses the
# regular equal "=" instead of the
# colon equal ":=". This is done so
# that variable expansion is defered
# until this variable is used.
DUMP_FMT = tr " " "\n" | sed 's/^\(\w\|-\)/    \1/; /^$$/d'

# This is the target that creates the
# final binary.
#
all: $(BIN_DIR)/$(BIN)

# This is the linker target that uses
# all of the compiled OBJS files and
# LDFLAGS to create the final binary.
#
$(BIN_DIR)/$(BIN): $(OBJS)
	@$(MKDIR) $$(dirname $@)
	$(LD) $(LDFLAGS) $^ -o $@

# This is the target that compiles all
# C files into object files under the
# $(OBJ_DIR).
$(OBJ_DIR)/%.o: %.c
	@$(MKDIR) $$(dirname $@)
	$(CC) $(CFLAGS) -o $@ $<

# This is the target that compiles all
# C++ files into object files under the
# $(OBJ_DIR).
$(OBJ_DIR)/%.o: %.cpp
	@$(MKDIR) $$(dirname $@)
	$(CXX) $(CXXFLAGS) -o $@ $<

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
DEPS := $(shell find $(OBJDIR) -name '*.d')
include $(DEPS)
