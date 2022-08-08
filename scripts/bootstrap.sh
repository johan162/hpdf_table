#!/usr/bin/env bash
## @file
## @brief Bootstrap the autotools environment and configure a build setup.
##
## @note This must be run when the source have been obtained by cloning the repo and
## requires a full installation of GNU autotools as a pre-requisite.
##
##  **Usage:**
##
##  bootstrap.sh [-q] [-h]
##
##  -c          : Clean **all** generated files. This is equivalent with cloning from the repo.<br>
##  -q          : Quiet <br>
##  -h          : Print help and exit
##
## See LICENSE file. (C) 2022 Johan Persson <johan162@gmail.com>
set -u

## The original directory from where this script is run
declare  ORIG_DIR="${PWD}"

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

# Figure out the basedir
setupbasedir() {
    # Sanity check to check if script is rung from top directory or one below
    [ -d "src" ] && PACKAGE_BASEDIR="${PWD}/"
    [ -d "../src" ]  &&  PACKAGE_BASEDIR="${PWD}/../"
    [ -z "${PACKAGE_BASEDIR}" ] && errlog "Please run from package top directory" && exit 1
}

# Check that this environment have autotools && libtool installed
chk_pre() {
    if ! command -v automake > /dev/null 2>&1  || ! command -v autoreconf > /dev/null 2>&1 ; then
        errlog "autotools and libtool must be installed to be able to recreate the build environment."
        exit 1
    fi
}

# Delete all files that can be regenerated
really_clean() {
    declare files="ar-lib compile depcomp install-sh missing aclocal.m4 Makefile.in src/Makefile.in examples/Makefile.in configure autom4te.cache ltmain.sh libtool m4 config.guess config.sub config.log config.status"
    if [ -e "${PACKAGE_BASEDIR}Makefile" ]; then
        cd "${PACKAGE_BASEDIR}" && make maintainer-clean > /dev/null 2>&1
    fi
    for val in ${files}; do
        echo "rm -rf ${PACKAGE_BASEDIR}${val}"
        [ -e "${PACKAGE_BASEDIR}${val}" ] && rm -rf "${PACKAGE_BASEDIR}${val}"
    done
    rm -f *.gz *.xz
}


# Bootstrap autoconf/automake environment
bootstrap() {
    if [ -f "${PACKAGE_BASEDIR}compile" -a -f "${PACKAGE_BASEDIR}install-sh" -a -f "${PACKAGE_BASEDIR}aclocal.m4" -a -f "${PACKAGE_BASEDIR}missing" -a -f "${PACKAGE_BASEDIR}libtool" ]; then
        infolog "autoreconf -i has already been run so only run autoreconf and configure"
        cd "${PACKAGE_BASEDIR}" && autoreconf && ./configure
    else
        infolog "Need to add the missing standard automake files"
        infolog "Running 'autoreconf' .. "
        cd "${PACKAGE_BASEDIR}" && autoreconf -i > /dev/null 2>&1
        infolog "configure .. "
        cd "${PACKAGE_BASEDIR}" && ./configure CFLAGS="-O3"
    fi
}

# Setup the directory we are running from
setupbasedir

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
