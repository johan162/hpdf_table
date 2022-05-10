AM_CFLAGS =  -pedantic -Wall -Werror -Wpointer-arith -Wstrict-prototypes \
-Wextra -Wshadow -Wno-error=unknown-pragmas -Werror=format -Wformat=2

FILES = example01 tut_ex01 tut_ex02 tut_ex02_1 tut_ex03 tut_ex04 tut_ex05 tut_ex06 tut_ex07 tut_ex08 \
        tut_ex09 tut_ex10 tut_ex11 tut_ex12 tut_ex13_1 tut_ex13_2 tut_ex14 tut_ex15 tut_ex15_1 tut_ex16_dash \
        tut_ex20

EXTRA_DIST = tests verify.sh unit_test.inc.h

noinst_bin_PROGRAMS = $(FILES)
noinst_bindir = $(pwd)
CLEANFILES =*~ $(FILES)

example01_LDADD = ../src/libhpdftbl.la
example01_DEPENDENCIES = ../src/libhpdftbl.la unit_test.inc.h

tut_ex01_LDADD = ../src/libhpdftbl.la
tut_ex01_DEPENDENCIES = ../src/libhpdftbl.la unit_test.inc.h

tut_ex02_LDADD = ../src/libhpdftbl.la
tut_ex02_DEPENDENCIES = ../src/libhpdftbl.la unit_test.inc.h

tut_ex02_1_LDADD = ../src/libhpdftbl.la
tut_ex02_1_DEPENDENCIES = ../src/libhpdftbl.la unit_test.inc.h

tut_ex03_LDADD = ../src/libhpdftbl.la
tut_ex03_DEPENDENCIES = ../src/libhpdftbl.la unit_test.inc.h

tut_ex04_LDADD = ../src/libhpdftbl.la
tut_ex04_DEPENDENCIES = ../src/libhpdftbl.la unit_test.inc.h

tut_ex05_LDADD = ../src/libhpdftbl.la
tut_ex05_DEPENDENCIES = ../src/libhpdftbl.la unit_test.inc.h

tut_ex06_LDADD = ../src/libhpdftbl.la
tut_ex06_DEPENDENCIES = ../src/libhpdftbl.la unit_test.inc.h

tut_ex07_LDADD = ../src/libhpdftbl.la
tut_ex07_DEPENDENCIES = ../src/libhpdftbl.la unit_test.inc.h

tut_ex08_LDADD = ../src/libhpdftbl.la
tut_ex08_DEPENDENCIES = ../src/libhpdftbl.la unit_test.inc.h

tut_ex09_LDADD = ../src/libhpdftbl.la
tut_ex09_DEPENDENCIES = ../src/libhpdftbl.la unit_test.inc.h

tut_ex10_LDADD = ../src/libhpdftbl.la
tut_ex10_DEPENDENCIES = ../src/libhpdftbl.la unit_test.inc.h

tut_ex11_LDADD = ../src/libhpdftbl.la
tut_ex11_DEPENDENCIES = ../src/libhpdftbl.la unit_test.inc.h

tut_ex12_LDADD = ../src/libhpdftbl.la
tut_ex12_DEPENDENCIES = ../src/libhpdftbl.la unit_test.inc.h

tut_ex13_1_LDADD = ../src/libhpdftbl.la
tut_ex13_1_DEPENDENCIES = ../src/libhpdftbl.la unit_test.inc.h

tut_ex13_2_LDADD = ../src/libhpdftbl.la
tut_ex13_2_DEPENDENCIES = ../src/libhpdftbl.la unit_test.inc.h

tut_ex14_LDADD = ../src/libhpdftbl.la
tut_ex14_DEPENDENCIES = ../src/libhpdftbl.la unit_test.inc.h

tut_ex15_LDADD = ../src/libhpdftbl.la
tut_ex15_DEPENDENCIES = ../src/libhpdftbl.la unit_test.inc.h

tut_ex15_1_LDADD = ../src/libhpdftbl.la
tut_ex15_1_DEPENDENCIES = ../src/libhpdftbl.la unit_test.inc.h

tut_ex16_dash_LDADD = ../src/libhpdftbl.la
tut_ex16_dash_DEPENDENCIES = ../src/libhpdftbl.la unit_test.inc.h

tut_ex20_LDADD = ../src/libhpdftbl.la
tut_ex20_DEPENDENCIES = ../src/libhpdftbl.la unit_test.inc.h

check-local:
	./verify.sh

clean-local:
	rm -rf out

