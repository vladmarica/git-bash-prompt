CC=g++
FLAGS=-Wall -Wextra -std=gnu++11

all:
	$(CC) $(FLAGS) -o git-bash-prompt prompt.cpp
