# ===============================================================================
# Autoconf script for the libhpdftbl library
# Use scripts/bootstrap.sh to bootstrap the build environment
# ===============================================================================
AC_PREREQ([2.69])
AC_INIT([libhpdftbl],[1.4.0-beta],[johan162@gmail.com])
AC_COPYRIGHT([Part of configure script (C) 2022 Johan Persson <johan162@gmail.com>])
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
examples/unit_test.inc.h
Makefile
src/Makefile
examples/Makefile
docs/Makefile
docs/Doxyfile

])

# The AC_OUTPUT strips files of the x mode so we need to restore it
AC_CONFIG_COMMANDS([set-xmode], [chmod +x examples/verify.sh scripts/docupload.sh])

AC_CANONICAL_HOST
case $host_os in
  darwin* )
		AM_CONDITIONAL(IS_LINUX, false)
		AM_CONDITIONAL(IS_OSX, true)
		AC_DEFINE(IS_OSX,1,[True if system type is Apple OSX])
		AC_DEFINE(IS_LINUX,0,[True if system type is Linux])
		# For OSX this is a separate library and we should check that it really works.
		# For Linux iconv is included in glibc-6 and is not a separate library we can check for
    	AC_CHECK_LIB(iconv, iconv_open,,AC_MSG_ERROR([No iconv library found. Please install iconv (or similar)]))
        ;;
  linux* )
		AM_CONDITIONAL(IS_LINUX, true)
		AM_CONDITIONAL(IS_OSX, false)
		AC_DEFINE(IS_OSX,0,[True if system type is Apple OSX])
		AC_DEFINE(IS_LINUX,1,[True if system type is Linux])
		;;
  *)
        AC_MSG_ERROR([Only Linux and OSX platforms are currently supported])
        ;;
esac

AC_LANG([C])
AC_PROG_INSTALL
AC_PROG_MAKE_SET

# Sanity check
AC_CHECK_HEADERS(hpdf.h,,AC_MSG_ERROR([libharu not installed hpdf.h missing]))

# Add double check that the library is actually working
# If exists this will also add -lhpdf to the LIBS
AC_CHECK_LIB(hpdf, HPDF_New,,AC_MSG_ERROR([No libhpdf found. Please install libhpdf-dev (or similar)]))

# Find out if we can compile with -ljansson or not.
# If exists this will also add -ljansson to the LIBS
#AC_CHECK_LIB(jansson, json_object_get,,AC_MSG_ERROR([libjansson not found]))
AC_CHECK_LIB(jansson, json_object_get,HAVE_LIBJANSSON="true",HAVE_LIBJANSSON="false")

if test "x$HAVE_LIBJANSSON" = "xtrue"; then
	AC_DEFINE(HAVE_LIBJANSSON,1,[True if system have json library jansson])
	LIBS+="-ljansson"
else
	AC_DEFINE(HAVE_LIBJANSSON,0,[True if system have json library jansson])
fi
AM_CONDITIONAL(have_libjansson, test "x$HAVE_LIBJANSSON" = "xtrue")

# We need to determine which ranlib to use in order to build our static library
# AC_PROG_RANLIB

# Check if Doxygen is installed
AC_CHECK_PROG(DOXYGEN,doxygen,yes,no)
AM_CONDITIONAL(have_doxygen, test "x$DOXYGEN" = "xyes")

# Check if pdflatex is installed (can we rebuild the PDF version of Doxygen docs
AC_CHECK_PROG(PDFLATEX,pdflatex,yes,no)
AM_CONDITIONAL(have_pdflatex, test "x$PDFLATEX" = "xyes")

# For use in Doxyfile.in
if test "x$PDFLATEX" = "xyes"; then
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
if test "x$HAVE_LIBJANSSON" = "xtrue"; then
	AC_MSG_NOTICE([  - Build configured with json export/import functions])
else
	AC_MSG_NOTICE([  - Build NOT configured with json export/import functions])
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

