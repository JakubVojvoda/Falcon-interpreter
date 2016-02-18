################################################################################
#
#  Falcon Interpreter
#
#  for Formal Languages and Compilers 2012/2013
#  by Varga Tomas, Vojvoda Jakub, Nesvadba Tomas, Spanko Jaroslav, Warzel Adam
#
################################################################################

# Make settings
PROG=build/ifalcon

CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -pedantic
LFLAGS=-lm

FILES=src/ifj12.c \
src/builtin.c \
src/builtin.h \
src/debug.c \
src/functions.c \
src/ial.c \
src/interpret.c \
src/lex.c \
src/synan.c \
src/types.c

#Main program
all:
	$(CC) $(CFLAGS) $(FILES) $(LFLAGS) -o $(PROG)

#Options
.PHONY: clean

clean:
	rm -f src/*~ src/*.o $(PROG)
