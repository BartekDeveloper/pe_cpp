# ----------------- #
#    VARIABLES      #
# ----------------- #

# Programs
GCC    = gcc
CXX    = g++
LD     = ld
NASM   = nasm

# Executable
EXE_NAME ?= main
TYPE     ?= debug

# Directories
SRC_DIRS    = ./src
OBJ_DIR     = ./compiled
OUT_DIR     = ./out/$(TYPE)
DOCS_DIR    = ./docs
INCLUDE_DIR = ./external/include
STATIC_DIR  = ./external/static
DYNAMIC_DIR = ./external/dynamic

# Files
EXE = $(OUT_DIR)/$(EXE_NAME)

# Find source files
SOURCES_C   = $(wildcard $(SRC_DIRS)/**/*.c)
SOURCES_CPP = $(wildcard $(SRC_DIRS)/**/*.cpp)
SOURCES_ASM = $(wildcard $(SRC_DIRS)/**/*.s)

# Check for main.c or main.cpp and set linker
ifneq ($(wildcard main.c),)
    SOURCES_C += main.c
    LINKER_CMD = $(GCC)
else
    SOURCES_CPP += main.cpp
    LINKER_CMD = $(CXX)
endif

# Create object file names
OBJECTS_C   = $(addprefix $(OBJ_DIR)/, $(notdir $(SOURCES_C:.c=.o)))
OBJECTS_CPP = $(addprefix $(OBJ_DIR)/, $(notdir $(SOURCES_CPP:.cpp=.o)))
OBJECTS_ASM = $(addprefix $(OBJ_DIR)/, $(notdir $(SOURCES_ASM:.s=.o)))
OBJECTS     = $(OBJECTS_C) $(OBJECTS_CPP) $(OBJECTS_ASM)

# VPATH for finding sources
VPATH = $(sort $(dir $(SOURCES_C) $(SOURCES_CPP) $(SOURCES_ASM)))

# Compiler flags
C_STD       = -std=c99
CXX_STD     = -std=c++17
DEBUG_FLAGS = -g -O0
RELEASE_FLAGS = -O3
CFLAGS      = $(C_STD) -I$(INCLUDE_DIR) -I./src/interfaces
CXXFLAGS    = $(CXX_STD) -I$(INCLUDE_DIR) -I./src/interfaces
LDFLAGS     = -L$(STATIC_DIR) -L$(DYNAMIC_DIR)
CPPFLAGS    = -DMAIN_PROJECT_VERSION=\"1.0.0\" -DMAIN_PROJECT_NAME=\"$(EXE_NAME)\" -DMAIN_PROJECT_AUTHOR=\"Zota\"

# Set flags based on TYPE
ifeq ($(TYPE),debug)
    CFLAGS   += $(DEBUG_FLAGS)
    CXXFLAGS += $(DEBUG_FLAGS)
else
    CFLAGS   += $(RELEASE_FLAGS)
    CXXFLAGS += $(RELEASE_FLAGS)
endif

# ----------------- #
#      RULES        #
# ----------------- #

.PHONY: all clean make_dirs interfaces clean_interfaces debug run release release_run docs copyright help

all: debug

debug:
	@$(MAKE) TYPE=debug $(EXE)

release:
	@$(MAKE) TYPE=release $(EXE)

run: debug
	@./$(EXE)

release_run: release
	@./$(EXE)

$(EXE): $(OBJECTS) | $(OUT_DIR)
	$(LINKER_CMD) $(CXXFLAGS) $(CPPFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	$(GCC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: %.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: %.s | $(OBJ_DIR)
	$(NASM) -f elf64 $< -o $@

make_dirs:
	@mkdir -p $(OBJ_DIR) ./out/debug ./out/release $(DOCS_DIR) ./src/interfaces

clean:
	@rm -rf $(OBJ_DIR)/* ./out/debug/* ./out/release/*

interfaces:
	@find ./src -type f -name "*.h" -exec cp {} ./src/interfaces \;

clean_interfaces:
	@rm -rf ./src/interfaces/*

docs:
	@doxygen -g
	@doxygen

copyright:
	@if [ -f LICENSE.md ]; then \
		for file in $(SOURCES_C) $(SOURCES_CPP) $(wildcard ./src/**/*.h) $(wildcard ./src/**/*.hpp) main.hpp shared.hpp; do \
			echo "/*" >> $$file; \
			cat LICENSE.md >> $$file; \
			echo "*/" >> $$file; \
		done \
	fi

help:
	@echo "Possible commands:"
	@echo "  make clean:      remove all .o files from ./compiled and ./out/{TYPE}"
	@echo "  make make_dirs:  create the ./compiled and ./out/{TYPE} directories and ./docs"
	@echo "  make interfaces: generate the interface files for the project"
	@echo "  make clean_interfaces: remove all .h files from ./src/interfaces"
	@echo "  make debug:       compile the project in debug mode"
	@echo "  make run:         make debug and run the created executable"
	@echo "  make release:     compile the project in release mode"
	@echo "  make release_run: make release and run the created executable"
	@echo "  make docs:        generate the documentation for the project"
	@echo "  make copyright:   append license to source files"
	@echo "  make help:        print all possible commands"

# Order-only prerequisites
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OUT_DIR):
	mkdir -p $(OUT_DIR)
