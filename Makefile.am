# Top level automake which gives the structure of the program layout

# Avoid making automake give errors if INSTALL, COPYING, NEWS etc. are missing
AUTOMAKE_OPTIONS = foreign


# Structure 
SUBDIRS = . src docs

if WITH_EXAMPLES
   SUBDIRS += examples
endif


# Additional clean up files
DISTCLEANFILES=.dep.inc
CLEANFILES=configure~

ACLOCAL_AMFLAGS=-I m4

EXTRA_DIST=scripts


