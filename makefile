SRC     := src
BUILD   := build
OBJ_DIR := $(BUILD)/obj
BIN_DIR := $(BUILD)/bin

TARGET := $(BIN_DIR)/forge

MAKEFLAGS += --no-print-directory

CC       := gcc
OBJFLAGS := -c -g -Wall -Wextra -Wpedantic -I$(SRC)
CFLAGS   := -Wall -Wextra -Wpedantic -g

SRC_C := $(wildcard $(SRC)/*.c)
SRC_S := $(wildcard $(SRC)/*.s)
OBJ_C := $(patsubst $(SRC)/%.c,$(OBJ_DIR)/%.o,$(SRC_C))
OBJ_S := $(patsubst $(SRC)/%.s,$(OBJ_DIR)/%.o,$(SRC_S))
ALLOBJ:= $(OBJ_C) $(OBJ_S)

PROGRAM_INPUT := --tokenization

.PHONY: all
all: clean build run

.PHONY: clean
clean:
	@echo "CLEANING DONE"
	@rm -rf $(BUILD)/*

.PHONY: build
build: $(ALLOBJ)
	@echo "LINKING DONE"
	@mkdir -p $(BIN_DIR)
	@$(CC) $(CFLAGS) $(ALLOBJ) -o $(TARGET)

# compile C sources to object files
$(OBJ_DIR)/%.o: $(SRC)/%.c
	@mkdir -p $(OBJ_DIR)
	@echo "COMPILING $< DONE"
	@$(CC) $(OBJFLAGS) -c $< -o $@

# compile assembly sources to object files
$(OBJ_DIR)/%.o: $(SRC)/%.s
	@mkdir -p $(OBJ_DIR)
	@echo "ASSEMBLING $< DONE"
	@$(CC) $(OBJFLAGS) -c $< -o $@

.PHONY: run
run:
	@clear
	@echo "RUNNING PROGRAM:"
	@$(TARGET) $(PROGRAM_INPUT)

.PHONY: debug
debug: clean build
	@clear
	@echo "RUNNING PROGRAM IN DEBUG MODE:"
	@gdb $(TARGET)