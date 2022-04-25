#!/usr/bin/env bash
# Script to both bootstrap the autoconf/automake environment as well as clean it to pristine shape
# See LICENSE file. (c) 2022 Johan Persson <johan162@gmail.com>
set -u

# Find out if we are running from the script directory
# or from the parent directory
if [ -f "bootstrap.sh" ]; then
    pdir="../"
else
    pdir="./"
fi

# Print usage
# Arg 1: name of script
usage() {
    echo "Bootstrap automake setup for libhpdftbl"
    echo "Usage:"
    echo "\$ $1 [-c]"
    echo "-c          : Clean all generated files"
    echo "-q          : Quiet"
    echo "-h          : Print help and exit"
}

quiet_flag=0
# User information
# Arg 1: Info text to display
infolog() {
    [ $quiet_flag -eq 0 ] && printf "Info: %s\n" "$1" 
}

# User information
# Arg 1: Info text to display
errlog() {
    [ $quiet_flag -eq 0 ] && printf "***ERROR***: %s\n" "$1" >&2
}

# Check that this environment have autotools && libtool installed
chk_pre() {
    if ! command -v glibtool > /dev/null 2>&1  || ! command -v glibtoolize > /dev/null 2>&1 \
    || ! command -v automake > /dev/null 2>&1  || ! command -v autoreconf > /dev/null 2>&1 ; then
        errlog "autotools and libtool must be installed to be able to recreate the build environment."
        exit 1
    fi
}

# Delete all files that can be regenerated
really_clean() {
    declare files="ar-lib compile depcomp install-sh missing aclocal.m4 Makefile.in src/Makefile.in examples/Makefile.in configure autom4te.cache ltmain.sh libtool m4 config.guess config.sub config.log config.status"
    if [ -e "${pdir}Makefile" ]; then
        cd "${pdir}" && make maintainer-clean > /dev/null 2>&1
    fi
    for val in ${files}; do
        [ -e "${pdir}${val}" ] && rm -rf "${pdir}${val}"
    done
    rm -f *.gz *.xz
}


# Bootstrap autoconf/automake environment
bootstrap() {
    if [ -f "${pdir}compile" -a -f "${pdir}install-sh" -a -f "${pdir}aclocal.m4" -a -f "${pdir}missing" -a -f "${pdir}libtool" ]; then
        infolog "automake -a has already been run so only run autoreconf and configure"
        cd "${pdir}" && autoreconf && ./configure
    else
        infolog  "Need to add the missing standard automake files"
        infolog "Running 'autoreconf' .. "
        cd "${pdir}" && autoreconf  > /dev/null 2>&1
        infolog "Running 'automake -ac' .. "
        cd "${pdir}" && automake -ac > /dev/null 2>&1
        infolog "Running 'glibtoolize -ci' .. "
        cd "${pdir}" && glibtoolize -ic > /dev/null 2>&1
        infolog "Running 'autoreconf' .. "
        cd "${pdir}" && autoreconf  > /dev/null 2>&1
        infolog "configure .. "
        cd "${pdir}" && ./configure
    fi
}

# Parse options and run program
while [[ $OPTIND -le "$#" ]]; do
    if getopts ch option; then
        case $option in
        c)
            really_clean
            exit 0
            ;;
        h)
            usage "$(basename $0)"
            exit 0
            ;;
         q)
            quiet_flag=1
            ;;
        [?])
            usage "$(basename $0)"
            exit 1
            ;;
        esac
    fi
done

# Make sure autotools && libtool are installed
chk_pre

# If no options then by default we bootstrap the library
bootstrap

# EOF
