#!/bin/bash
#
# Generate the compile_commands.json file for VSCode intellisense using the
# bear utility.

# If passed the --hal option, generate the HAL version compile commands.
INCLUDE_HAL=0
if [[ -n $1 && $1 == "--hal" ]]; then
    INCLUDE_HAL=1
fi

# Targets needed in the Makefile
CLEAN="clean"
FULL_BUILD="all"

# This script requires using the Makefile in the root of the Git repo. Use the
# Git to get the name of the root directory and change to it.
cd "$(git rev-parse --show-toplevel)" || exit 1

# Do a clean so we can run all the compile commands.
make "$CLEAN"

# Use the bear utility to record all the compiler commands used in the build.
bear --output .vscode/compile_commands.json -- make USE_HAL="$INCLUDE_HAL" "$FULL_BUILD"