#!/usr/bin/env bash
## @file
## @brief Setup a build environment for production build.
##
##  **Usage:**
##
##  stdbld.sh [-q] [-h]
##
##  -q          : Quiet <br>
##  -h          : Print help and exit
##
## See LICENSE file. (C) 2022 Johan Persson <johan162@gmail.com>

## The original directory from where this script is run
declare -r ORIG_DIR="${PWD}"

# Don't make modifications beyond this point

## @brief Suppress output
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
stdsetup() {
  cd ${PACKAGE_BASEDIR}
  ./configure CFLAGS="-O3" --enable-silent-rules
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
stdsetup
