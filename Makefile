# Compiler
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -Iinclude 
LDFLAGS= -lreadline

# Directories
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# Target
TARGET = $(BIN_DIR)/app

# Find all source files
SRCS = $(wildcard $(SRC_DIR)/*.cpp)

# Generate corresponding object files
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

# Default rule
all: $(TARGET)

# Link
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# Compile
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Rebuild
re: clean all

.PHONY: all clean re
