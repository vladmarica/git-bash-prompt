CC=g++
FLAGS=-Wall -Wextra -std=gnu++17

all:
	$(CC) $(FLAGS) -o git-bash-prompt prompt.cpp
