# Basic makefile to build example for usage of HPDF_TABLE example.
# This Makefile is only meant to be run on OS X
# Note: Assumes that libhpdf-2 is installed

SOURCES=hpdf_errstr.c hpdf_grid.c hpdf_table.c hpdf_table_widget.c hpdf_table_example1.c
HEADERS=hpdf_errstr.h hpdf_grid.h hpdf_table.h hpdf_table_widget.h

CFLAGS=--std=c99 -ggdb -O2 -Wall -Wpedantic
LNFLAGS=-lm /usr/local/Cellar/libharu/2.2.1_1/lib/libhpdf-2.2.1.dylib /usr/lib/libiconv.2.dylib

.PHONY: all
all: ex1

ex1: ${SOURCES} ${HEADERS}
	gcc -o $@ ${CFLAGS} ${LNFLAGS} ${SOURCES}

.PHONY: clean
clean:
	rm -f ex1

.PHONY: really-clean
really-clean:
	rm -rf ex1 ex1.dSYM
