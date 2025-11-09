CC = gcc
CCFLAGS = -fsanitize=address -g
SRC = $(wildcard *.c)

all: compile run

clean:
	rm -rf Main

compile: clean
	$(CC) $(CCFLAGS) $(SRC) -o Main

run:
	./Main $(cat ./input.txt)
