AM_CFLAGS =  -pedantic -Wall -Werror -Wpointer-arith -Wstrict-prototypes \
-Wextra -Wshadow -Wno-error=unknown-pragmas -Werror=format -Wformat=2 -std=gnu99

FILES = example01 tut_ex00 tut_ex01 tut_ex02 tut_ex02_1 tut_ex03 tut_ex04 tut_ex05 tut_ex06 tut_ex07 tut_ex08 \
        tut_ex09 tut_ex10 tut_ex11 tut_ex12 tut_ex13_1 tut_ex13_2 tut_ex14 tut_ex15 tut_ex15_1 tut_ex16_dash \
        tut_ex20 tut_ex30

if have_libjansson
FILES+=tut_ex40 tut_ex41
endif

EXTRA_DIST = tests verify.sh unit_test.inc.h.in

noinst_bin_PROGRAMS = $(FILES)
noinst_bindir = $(pwd)
CLEANFILES =*~ $(FILES)
HPDF_LIB=../src/libhpdftbl.la
DEPS=../src/libhpdftbl.la unit_test.inc.h

example01_LDADD = ${HPDF_LIB}
example01_DEPENDENCIES = ${DEPS}

tut_ex00_LDADD = ${HPDF_LIB}
tut_ex00_DEPENDENCIES = ${HPDF_LIB}

tut_ex01_LDADD = ${HPDF_LIB}
tut_ex01_DEPENDENCIES = ${HPDF_LIB}

tut_ex02_LDADD = ${HPDF_LIB}
tut_ex02_DEPENDENCIES = ${HPDF_LIB}

tut_ex02_1_LDADD = ${HPDF_LIB}
tut_ex02_1_DEPENDENCIES = ${HPDF_LIB}

tut_ex03_LDADD = ${HPDF_LIB}
tut_ex03_DEPENDENCIES = ${HPDF_LIB}

tut_ex04_LDADD = ${HPDF_LIB}
tut_ex04_DEPENDENCIES = ${HPDF_LIB}

tut_ex05_LDADD = ${HPDF_LIB}
tut_ex05_DEPENDENCIES = ${HPDF_LIB}

tut_ex06_LDADD = ${HPDF_LIB}
tut_ex06_DEPENDENCIES = ${HPDF_LIB}

tut_ex07_LDADD = ${HPDF_LIB}
tut_ex07_DEPENDENCIES = ${HPDF_LIB}

tut_ex08_LDADD = ${HPDF_LIB}
tut_ex08_DEPENDENCIES = ${HPDF_LIB}

tut_ex09_LDADD = ${HPDF_LIB}
tut_ex09_DEPENDENCIES = ${HPDF_LIB}

tut_ex10_LDADD = ${HPDF_LIB}
tut_ex10_DEPENDENCIES = ${HPDF_LIB}

tut_ex11_LDADD = ${HPDF_LIB}
tut_ex11_DEPENDENCIES = ${HPDF_LIB}

tut_ex12_LDADD = ${HPDF_LIB}
tut_ex12_DEPENDENCIES = ${HPDF_LIB}

tut_ex13_1_LDADD = ${HPDF_LIB}
tut_ex13_1_DEPENDENCIES = ${HPDF_LIB}

tut_ex13_2_LDADD = ${HPDF_LIB}
tut_ex13_2_DEPENDENCIES = ${HPDF_LIB}

tut_ex14_LDADD = ${HPDF_LIB}
tut_ex14_DEPENDENCIES = ${HPDF_LIB}

tut_ex15_LDADD = ${HPDF_LIB}
tut_ex15_DEPENDENCIES = ${HPDF_LIB}

tut_ex15_1_LDADD = ${HPDF_LIB}
tut_ex15_1_DEPENDENCIES = ${HPDF_LIB}

tut_ex16_dash_LDADD = ${HPDF_LIB}
tut_ex16_dash_DEPENDENCIES = ${HPDF_LIB}

tut_ex20_LDADD = ${HPDF_LIB}
tut_ex20_DEPENDENCIES = ${HPDF_LIB}

tut_ex30_LDADD = ${HPDF_LIB}
tut_ex30_DEPENDENCIES = ${HPDF_LIB}

if have_libjansson
tut_ex40_LDADD = ${HPDF_LIB}
tut_ex40_DEPENDENCIES = ${HPDF_LIB}
tut_ex40_LDFLAGS = -ljansson

tut_ex41_LDADD = ${HPDF_LIB}
tut_ex41_DEPENDENCIES = ${HPDF_LIB}
tut_ex41_LDFLAGS = -ljansson
endif

check-local:
	./verify.sh

clean-local:
	rm -rf out

