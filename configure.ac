# ===============================================================================
# Autoconf script for the libhpdftbl library
# Use scripts/bootstrap.sh to bootstrap the build environment
# ===============================================================================
AC_PREREQ([2.69])
AC_INIT([libhpdftbl],[1.1.1],[johan162@gmail.com])
AC_COPYRIGHT([Part of configure script Copyright 2022 Johan Persson <johan162@gmail.com>])
AC_PREFIX_DEFAULT(/usr/local)
AM_INIT_AUTOMAKE([dist-xz silent-rules])
AM_SILENT_RULES
AM_PROG_AR
AC_CONFIG_HEADERS([src/config.h])
LT_INIT
AC_CONFIG_MACRO_DIRS([m4])

AC_CONFIG_FILES([
scripts/docupload.sh
examples/verify.sh
Makefile
src/Makefile
examples/Makefile
docs/Makefile
docs/Doxyfile
])

# The AC_OUTPUT strips files of the x mode so we need to restore it
AC_CONFIG_COMMANDS([set-xmode], [chmod +x examples/verify.sh scripts/docupload.sh])

SYSTEM_NAME=`uname -s`

# Add the system type (Linux or OS X) in config.h
if test x$SYSTEM_NAME = xDarwin; then
    AC_DEFINE(IS_OSX,1,[True if system type is Apple OSX])
else
    AC_DEFINE(IS_OSX,0,[True if system type is Apple OSX])
fi
AM_CONDITIONAL(is_osx,[test x$SYSTEM_NAME = xDarwin])
AM_CONDITIONAL(is_linux,[test x$SYSTEM_NAME = xLinux])

AC_LANG([C])
AC_PROG_INSTALL
AC_PROG_MAKE_SET

# Sanity check
AC_CHECK_HEADERS(hpdf.h,,AC_MSG_ERROR([libharu not installed hpdf.h missing]))
AC_CHECK_HEADERS(iconv.h,,AC_MSG_ERROR([libiconv not installed iconv.h missing]))

# Add double check that the library is actually working
AC_CHECK_LIB(hpdf, HPDF_New,,AC_MSG_ERROR([No Haru PDF library found. Please install libhpdf-dev (or similar)]))

# For OSX this is a separate library and we should check that it really works.
# For Linux iconv is included in glibc-6 and is not a separate library we can check for
if test x$SYSTEM_NAME = xDarwin; then
    AC_CHECK_LIB(iconv, iconv_open,,AC_MSG_ERROR([No iconv library found. Please install iconv (or similar)]))
fi

# We need to determine which ranlib to use in order to build our static library
# AC_PROG_RANLIB

# Check if Doxygen is installed
AC_CHECK_PROG(DOXYGEN,doxygen,yes,no)
AM_CONDITIONAL(have_doxygen, test "$DOXYGEN" = "yes")

# Check if pdflatex is installed (can we rebuild the PDF version of Doxygen docs
AC_CHECK_PROG(PDFLATEX,pdflatex,yes,no)
AM_CONDITIONAL(have_pdflatex, test "$PDFLATEX" = "yes")

# For use in Doxyfile.in
if test "$PDFLATEX" = "yes"; then
	AC_SUBST([USE_PDFLATEX],[YES])
else
	AC_SUBST([USE_PDFLATEX],[NO])
fi

# Output all generated files
AC_OUTPUT

AC_MSG_NOTICE([--------------------------------------------------------------------------------])
AC_MSG_NOTICE([INSTALLATION SUMMARY:])

if test x$SYSTEM_NAME = xDarwin; then
    AC_MSG_NOTICE([  - Build configured for OSX.])
else
    AC_MSG_NOTICE([  - Build configured for ${SYSTEM_NAME}.])
fi
if test x$DOXYGEN = xyes; then
    AC_MSG_NOTICE([  - Can rebuild HTML docs with Doxygen.])
    if test x$PDFLATEX = xyes; then
        AC_MSG_NOTICE([  - Can also create PDF docs (have LaTeX).])
    fi
else
    AC_MSG_NOTICE([  - Can NOT rebuild docs. No Doxygen installed.])
fi
AC_MSG_NOTICE([  - Installing to ${prefix}])

AC_MSG_NOTICE([--------------------------------------------------------------------------------])

