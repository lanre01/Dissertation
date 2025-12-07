# Compiler and flags
CXX := clang++
CXXFLAGS := -std=c++20 -Wall -Wextra -O2 -I./src

# Directories
SRC_DIR := src
BUILD_DIR := build
BIN_DIR := bin

# Source files
MAIN_SRC := $(SRC_DIR)/main.cpp
#HEAP_SRCS := $(wildcard $(SRC_DIR)/Heap/*.cpp)
#BEAP_SRCS := $(wildcard $(SRC_DIR)/Beap/*.cpp)
#NBEAP_SRCS := $(wildcard $(SRC_DIR)/nBeap/*.cpp)

UNIT_TEST_SRCS := $(wildcard test/unit/*.cpp)

# Benchmark sources (3 files total)
BEAP_BENCH_SRC := test/benchmark/benchmark_beap.cpp
NBEAP_BENCH_SRC := test/benchmark/benchmark_nbeap.cpp
HEAP_BENCH_SRC := test/benchmark/benchmark_heap.cpp
BST_BENCH_SRC := test/benchmark/benchmark_bst.cpp

# Object files
MAIN_OBJ := $(BUILD_DIR)/main.o
#HEAP_OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(HEAP_SRCS))
#BEAP_OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(BEAP_SRCS))
#NBEAP_OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(NBEAP_SRCS))

UNIT_TEST_OBJS := $(patsubst test/unit/%.cpp,$(BUILD_DIR)/test/unit/%.o,$(UNIT_TEST_SRCS))

# Benchmark object files
BEAP_BENCH_OBJ := $(BUILD_DIR)/test/benchmark/benchmark_beap.o
NBEAP_BENCH_OBJ := $(BUILD_DIR)/test/benchmark/benchmark_nbeap.o
HEAP_BENCH_OBJ := $(BUILD_DIR)/test/benchmark/benchmark_heap.o
BST_BENCH_OBJ := $(BUILD_DIR)/test/benchmark/benchmark_bst.o 

ALL_OBJS := $(MAIN_OBJ) # $(HEAP_OBJS) $(BEAP_OBJS) $(NBEAP_OBJS)

# Executables
TARGET := $(BIN_DIR)/dissertation
UNIT_TEST_TARGET := $(BIN_DIR)/unit_tests

BEAP_BENCH_TARGET := $(BIN_DIR)/bench_beap
NBEAP_BENCH_TARGET := $(BIN_DIR)/bench_nbeap
HEAP_BENCH_TARGET := $(BIN_DIR)/bench_heap
BST_BENCH_TARGET := $(BIN_DIR)/bench_bst

# Production code objects (without main)
PRODUCTION_OBJS := $(filter-out $(BUILD_DIR)/main.o, $(ALL_OBJS))

# Default target
all: $(TARGET)

# ============================================================
# Build executables
# ============================================================

$(TARGET): $(ALL_OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $(ALL_OBJS)

$(UNIT_TEST_TARGET): $(UNIT_TEST_OBJS) $(PRODUCTION_OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lgtest -lgtest_main -pthread

# Beap benchmark
$(BEAP_BENCH_TARGET): $(BEAP_BENCH_OBJ) $(PRODUCTION_OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lbenchmark -lpthread

# nBeap benchmark
$(NBEAP_BENCH_TARGET): $(NBEAP_BENCH_OBJ) $(PRODUCTION_OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lbenchmark -lpthread

# heap benchmark
$(HEAP_BENCH_TARGET): $(HEAP_BENCH_OBJ) $(PRODUCTION_OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lbenchmark -lpthread

# bst benchmark
$(BST_BENCH_TARGET): $(BST_BENCH_OBJ) $(PRODUCTION_OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lbenchmark -lpthread

# ============================================================
# Compilation rules
# ============================================================

$(BUILD_DIR)/main.o: $(MAIN_SRC) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/Heap/%.o: $(SRC_DIR)/Heap/%.cpp | $(BUILD_DIR)/Heap
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/Beap/%.o: $(SRC_DIR)/Beap/%.cpp | $(BUILD_DIR)/Beap
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/nBeap/%.o: $(SRC_DIR)/nBeap/%.cpp | $(BUILD_DIR)/nBeap
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/test/unit/%.o: test/unit/%.cpp | $(BUILD_DIR)/test/unit
	$(CXX) $(CXXFLAGS) -I./src -c $< -o $@

# Benchmark compilation
$(BUILD_DIR)/test/benchmark/benchmark_beap.o: $(BEAP_BENCH_SRC) | $(BUILD_DIR)/test/benchmark
	$(CXX) $(CXXFLAGS) -I./src -c $< -o $@

$(BUILD_DIR)/test/benchmark/benchmark_nbeap.o: $(NBEAP_BENCH_SRC) | $(BUILD_DIR)/test/benchmark
	$(CXX) $(CXXFLAGS) -I./src -c $< -o $@

$(BUILD_DIR)/test/benchmark/benchmark_heap.o: $(HEAP_BENCH_SRC) | $(BUILD_DIR)/test/benchmark
	$(CXX) $(CXXFLAGS) -I./src -c $< -o $@

$(BUILD_DIR)/test/benchmark/benchmark_bst.o: $(BST_BENCH_SRC) | $(BUILD_DIR)/test/benchmark
	$(CXX) $(CXXFLAGS) -I./src -c $< -o $@

# ============================================================
# Run commands
# ============================================================

run: $(TARGET)
	./$(TARGET)

run-unit-tests: $(UNIT_TEST_TARGET)
	./$(UNIT_TEST_TARGET)

run-bench-beap: $(BEAP_BENCH_TARGET)
	./$(BEAP_BENCH_TARGET)

run-bench-nbeap: $(NBEAP_BENCH_TARGET)
	./$(NBEAP_BENCH_TARGET)

run-bench-heap: $(HEAP_BENCH_TARGET)
	./$(HEAP_BENCH_TARGET)

run-bench-bst: $(BST_BENCH_TARGET)
	./$(BST_BENCH_TARGET)

run-bench-all: run-bench-heap run-bench-beap run-bench-nbeap 

# Debug build
debug: CXXFLAGS += -g -DDEBUG
debug: $(TARGET)

# Directory creation
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

$(BUILD_DIR)/test:
	mkdir -p $(BUILD_DIR)/test

$(BUILD_DIR)/test/benchmark: | $(BUILD_DIR)/test
	mkdir -p $(BUILD_DIR)/test/benchmark

$(BUILD_DIR)/test/unit: | $(BUILD_DIR)/test
	mkdir -p $(BUILD_DIR)/test/unit

# Clean
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: all clean run debug run-unit-tests run-bench-beap run-bench-nbeap run-bench-heap run-bench-all
