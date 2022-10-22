# TOOLS
CC    := gcc -c -xc
CXX   := gcc -c -xc++
LD    := gcc
AR    := ar
RM    := rm -rf
MKDIR := mkdir -p

# OUTPUT NAME
BIN := sbc

# DIRECTORIES
BIN_DIR  := bin
OBJ_DIR  := obj
SRC_DIRS := src
INC_DIRS := include

SRC_FILES := $(foreach dir, $(SRC_DIRS), $(shell find $(dir) -name '*.cpp'))
SRC_FILES += $(foreach dir, $(SRC_DIRS), $(shell find $(dir) -name '*.c')) 

OBJS := $(filter %.o, $(SRC_FILES:.c=.o) $(SRC_FILES:.cpp=.o))
OBJS := $(foreach obj, $(OBJS), $(addprefix $(OBJ_DIR)/, $(obj)))

INC_VARS := $(foreach dir, $(INC_DIRS), $(addprefix -I, $(dir)))

# COMPILER FLAGS
COMFLAGS := -Wall
COMFLAGS += -Wextra
COMFLAGS += -Wshadow
COMFLAGS += -ffunction-sections
COMFLAGS += -fdata-sections
COMFLAGS += -MMD
COMFLAGS += -MP
COMFLAGS += $(INC_VARS)

CFLAGS := -std=c99

CXXFLAGS := -std=c++11

# LINKER FLAGS
LDFLAGS := -Wl,--gc-sections
LDFLAGS += -lm
LDFLAGS += -lstdc++

all: $(BIN_DIR)/$(BIN)

$(BIN_DIR)/$(BIN): $(OBJS)
	@$(MKDIR) $$(dirname $@)
	$(LD) $(LDFLAGS) $? -o $@

$(OBJ_DIR)/%.o: %.c
	@$(MKDIR) $$(dirname $@)
	$(CC) $(CFLAGS) $(COMFLAGS) -o $@ $<

$(OBJ_DIR)/%.o: %.cpp
	@$(MKDIR) $$(dirname $@)
	$(CXX) $(CXXFLAGS) $(COMFLAGS) -o $@ $<

.PHONY: clean
clean:
	$(RM) $(OBJ_DIR)
	$(RM) $(BIN_DIR)

DEPS := $(wildcard $(OBJDIR)/*.d)
ifneq ($(DEPS),)
include $(DEPS)
endif
