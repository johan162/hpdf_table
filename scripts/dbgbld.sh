#!/usr/bin/env bash
## @file
## @brief Setup a build environment for debugging.
##
## In order for easy debugging this means that the debug configuration
## will only build static library in order to be able to include it in
## the binaries (e.g. the example programs). With dynamic libraries not yet
## installed the libtools will build wrapper shell scripts which cannot be
## debugged.
##
##  **Usage:**
##
##  dbgbld.sh [-q] [-h]
##
##  -q          : Quiet <br>
##  -h          : Print help and exit
##
## See LICENSE file. (C) 2022 Johan Persson <johan162@gmail.com>

## The original directory from where this script is run
declare -r ORIG_DIR="${PWD}"

# Don't make modifications beyond this point

declare -i quiet_flag=0
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


# Print usage
# Arg 1: name of script
usage() {
    echo "Setup a debug environment using static library"
    echo "Usage:"
    echo "\$ $1 [-q] [-h]"
    echo "-q          : Quiet"
    echo "-h          : Print help and exit"
}

# Figure out the basedir
setupbasedir() {
    # Sanity check to check if script is rung from top directory or one below
    [ -d "scripts" ] && PACKAGE_BASEDIR="${PWD}"
    [ -d "../scripts" ]  &&  PACKAGE_BASEDIR="${PWD}/.."
    [ -z $PACKAGE_BASEDIR ] && errlog "Please run from package top directory" && exit 1
}

# Configure build system for debugging
dbgsetup() {
  cd ${PACKAGE_BASEDIR}
  ./configure --disable-shared CFLAGS="-O0 -ggdb"
  make clean
  make
}

# Parse options and run program
while [[ $OPTIND -le "$#" ]]; do
    if getopts ch option; then
        case $option in
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

# Setup basedir
setupbasedir

# Do the work
dbgsetup
