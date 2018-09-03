CC=g++
FLAGS=-Wall -Wextra -std=gnu++17
BIN_DIR=./bin
SRC_DIR=./src
OBJ_DIR=$(BIN_DIR)/obj

SOURCES := $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))

all: dirs $(OBJECTS)
	$(CC) $(FLAGS) $(OBJECTS) -o $(BIN_DIR)/git-bash-prompt.exe

dirs:
	mkdir -p $(BIN_DIR)
	mkdir -p $(OBJ_DIR)
	mkdir -p $(SRC_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(FLAGS) -I$(SRC_DIR) -c $< -o $@
