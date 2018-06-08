# Basic makefile to build example of usage of HPDF_TABLE example.
#
# System: GNU Makefile for generic Unix system
# Note: Assumes libhpdf have been installed
# Author: Johan Persson <johan162@gmail.com>

HEADERS:=$(wildcard *.h)
OBJECTS:=$(patsubst %.c,%.o,$(wildcard *.c))
CC=gcc

CFLAGS=--std=gnu99 -ggdb -O2 -Wall -Wpedantic -Wextra -Wpointer-arith
LNFLAGS=-lhpdf -lm

.PHONY: all
all: ex1

# Having each object file dependent on all headers is over the top but
# in this small example it doesn't matter and simplifies this manual
# makefile.
%.o: %.c ${HEADERS}
	${CC} -c -o $@ $< ${CFLAGS} 

ex1: ${OBJECTS}
	${CC} -o $@ $^ ${LNFLAGS} 


.PHONY: clean
clean:
	rm -f ex1 ${OBJECTS}


.PHONY: really-clean
really-clean:
	rm -rf ex1 ${OBJECTS} ex1.dSYM
