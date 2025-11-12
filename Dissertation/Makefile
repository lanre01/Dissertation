# Compiler and flags
CXX := clang++
CXXFLAGS := -std=c++20 -Wall -Wextra -O2 -I./src
# For clang, use: CXX := clang++

# Directories
SRC_DIR := src
BUILD_DIR := build
BIN_DIR := bin

# Source files
MAIN_SRC := $(SRC_DIR)/main.cpp
HEAP_SRCS := $(wildcard $(SRC_DIR)/Heap/*.cpp)
BEAP_SRCS := $(wildcard $(SRC_DIR)/Beap/*.cpp)
NBEAP_SRCS := $(wildcard $(SRC_DIR)/nBeap/*.cpp)

# Test and benchmark files
UNIT_TEST_SRCS := $(wildcard test/unit/*.cpp)
BENCH_SRCS := $(wildcard test/benchmark/*.cpp)

ALL_SRCS := $(MAIN_SRC) $(HEAP_SRCS) $(BEAP_SRCS) $(NBEAP_SRCS)

# Object files
MAIN_OBJ := $(BUILD_DIR)/main.o
HEAP_OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(HEAP_SRCS))
BEAP_OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(BEAP_SRCS))
NBEAP_OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(NBEAP_SRCS))

# Test object files
UNIT_TEST_OBJS := $(patsubst test/unit/%.cpp,$(BUILD_DIR)/test/unit/%.o,$(UNIT_TEST_SRCS))
BENCH_OBJS := $(patsubst test/benchmark/%.cpp,$(BUILD_DIR)/test/benchmark/%.o,$(BENCH_SRCS))

ALL_OBJS := $(MAIN_OBJ) $(HEAP_OBJS) $(BEAP_OBJS) $(NBEAP_OBJS)

# Target executables
TARGET := $(BIN_DIR)/dissertation
UNIT_TEST_TARGET := $(BIN_DIR)/unit_tests
BENCH_TARGET := $(BIN_DIR)/benchmarks

# Production code objects (without main)
PRODUCTION_OBJS := $(filter-out $(BUILD_DIR)/main.o, $(ALL_OBJS))

# Default target
all: $(TARGET)

# Create executables
$(TARGET): $(ALL_OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $(ALL_OBJS)

$(UNIT_TEST_TARGET): $(UNIT_TEST_OBJS) $(PRODUCTION_OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lgtest -lgtest_main -pthread

$(BENCH_TARGET): $(BENCH_OBJS) $(PRODUCTION_OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile main.cpp
$(BUILD_DIR)/main.o: $(MAIN_SRC) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile Heap files
$(BUILD_DIR)/Heap/%.o: $(SRC_DIR)/Heap/%.cpp | $(BUILD_DIR)/Heap
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile Beap files
$(BUILD_DIR)/Beap/%.o: $(SRC_DIR)/Beap/%.cpp | $(BUILD_DIR)/Beap
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile nBeap files
$(BUILD_DIR)/nBeap/%.o: $(SRC_DIR)/nBeap/%.cpp | $(BUILD_DIR)/nBeap
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile test files
$(BUILD_DIR)/test/unit/%.o: test/unit/%.cpp | $(BUILD_DIR)/test/unit
	$(CXX) $(CXXFLAGS) -I./src -c $< -o $@

$(BUILD_DIR)/test/benchmark/%.o: test/benchmark/%.cpp | $(BUILD_DIR)/test/benchmark
	$(CXX) $(CXXFLAGS) -I./src -c $< -o $@

# Create directories
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/Heap: | $(BUILD_DIR)
	mkdir -p $(BUILD_DIR)/Heap

$(BUILD_DIR)/Beap: | $(BUILD_DIR)
	mkdir -p $(BUILD_DIR)/Beap

$(BUILD_DIR)/nBeap: | $(BUILD_DIR)
	mkdir -p $(BUILD_DIR)/nBeap

$(BUILD_DIR)/test/unit: | $(BUILD_DIR)/test
	mkdir -p $(BUILD_DIR)/test/unit

$(BUILD_DIR)/test/benchmark: | $(BUILD_DIR)/test
	mkdir -p $(BUILD_DIR)/test/benchmark

$(BUILD_DIR)/test:
	mkdir -p $(BUILD_DIR)/test

# Build targets
unit-tests: $(UNIT_TEST_TARGET)

benchmarks: $(BENCH_TARGET)

test: unit-tests benchmarks

# Run commands
run: $(TARGET)
	./$(TARGET)

run-unit-tests: unit-tests
	./$(UNIT_TEST_TARGET)

run-benchmarks: benchmarks
	./$(BENCH_TARGET)

# Debug build
debug: CXXFLAGS += -g -DDEBUG
debug: $(TARGET)

# Show file structure
info:
	@echo "Source files:"
	@echo $(ALL_SRCS)
	@echo "Object files:"
	@echo $(ALL_OBJS)
	@echo "Unit test sources:"
	@echo $(UNIT_TEST_SRCS)
	@echo "Benchmark sources:"
	@echo $(BENCH_SRCS)

# Clean build
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: all clean run debug info unit-tests benchmarks test run-unit-tests run-benchmarks