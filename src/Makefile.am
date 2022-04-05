AM_CFLAGS = -pedantic -Wall -Werror -Wextra -Wpointer-arith -Wstrict-prototypes -Wno-error=unknown-pragmas

lib_LTLIBRARIES = libhpdftbl.la
libhpdftbl_la_SOURCES = hpdftbl_errstr.c hpdftbl_grid.c hpdftbl.c hpdftbl_widget.c
libhpdftbl_la_LDFLAGS = -version-info 1:0:0
include_HEADERS = hpdftbl.h

CLEANFILES = *~

DISTCLEANFILES=*.Po configure.h

clean-local:
	@rm -rf .deps

