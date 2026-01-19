# Makefile for PEBL Launcher
# Copyright (c) 2026 Shane T. Mueller
# Licensed under GPL

# Version
PEBL_VERSION = 2.3

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g -O0
CXXFLAGS += -I../../../libs/imgui
CXXFLAGS += -I../../../libs/imgui/backends
CXXFLAGS += -I../../../libs  # For nlohmann/json header-only library
CXXFLAGS += -I../../utility
CXXFLAGS += -DENABLE_BINRELOC
CXXFLAGS += -DPEBL_VERSION=\"$(PEBL_VERSION)\"
CXXFLAGS += -DPREFIX=\"/usr/local\"

# SDL2 flags
SDL2_CFLAGS = $(shell sdl2-config --cflags)
SDL2_LIBS = $(shell sdl2-config --libs) -lSDL2_image

CXXFLAGS += $(SDL2_CFLAGS)
LIBS = $(SDL2_LIBS) -lzip

# Output
TARGET = ../../../bin/pebl-launcher
OBJ_DIR = obj

# Source files
LAUNCHER_SOURCES = \
	PEBLLauncher.cpp \
	LauncherUI.cpp \
	LauncherConfig.cpp \
	ExperimentRunner.cpp \
	Study.cpp \
	Chain.cpp \
	WorkspaceManager.cpp \
	SnapshotManager.cpp \
	ZipExtractor.cpp \
	../../utility/BinReloc.cpp

IMGUI_SOURCES = \
	../../../libs/imgui/imgui.cpp \
	../../../libs/imgui/imgui_draw.cpp \
	../../../libs/imgui/imgui_widgets.cpp \
	../../../libs/imgui/imgui_tables.cpp \
	../../../libs/imgui/backends/imgui_impl_sdl2.cpp \
	../../../libs/imgui/backends/imgui_impl_sdlrenderer2.cpp

TEXTEDITOR_SOURCES = \
	TextEditor.cpp

SOURCES = $(LAUNCHER_SOURCES) $(IMGUI_SOURCES) $(TEXTEDITOR_SOURCES)

# Object files - place in OBJ_DIR with flattened names
OBJS = $(addprefix $(OBJ_DIR)/, $(notdir $(SOURCES:.cpp=.o)))

# Build rules
all: $(TARGET)

# Create obj directory
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(TARGET): $(OBJS)
	@echo "Linking $@..."
	$(CXX) $(OBJS) -o $(TARGET) $(LIBS)
	@echo "Build complete: $(TARGET)"

# Pattern rules for different source directories
$(OBJ_DIR)/%.o: %.cpp | $(OBJ_DIR)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: ../../utility/%.cpp | $(OBJ_DIR)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: ../../../libs/imgui/%.cpp | $(OBJ_DIR)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: ../../../libs/imgui/backends/%.cpp | $(OBJ_DIR)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: ../../../libs/%.cpp | $(OBJ_DIR)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

run: $(TARGET)
	$(TARGET)

# Compile just the study system modules (for testing)
study-system: $(OBJ_DIR)/Study.o $(OBJ_DIR)/Chain.o $(OBJ_DIR)/WorkspaceManager.o $(OBJ_DIR)/SnapshotManager.o
	@echo "Study system modules compiled successfully"

.PHONY: all clean run study-system
