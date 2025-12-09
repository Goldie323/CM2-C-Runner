CC = gcc
CCFLAGS = -fsanitize=address -g

COMMON_SRC := \
	Main.c \
	GateFuncs.c \
	BlockCreation.c \
	SavestringParse.c \
	Util.c

ifeq ($(THREADS),0)
    TICKS_SRC = SingleTicks.c
    LIBS      =
else
    TICKS_SRC = ThreadedTicks.c
    LIBS      = -lpthread
endif

SRC := $(COMMON_SRC) $(TICKS_SRC)

all: compile run

clean:
	rm -f Main

compile: clean
	$(CC) $(CCFLAGS) $(SRC) $(LIBS) -o Main

run:
	./Main $(shell cat ./input.txt)
