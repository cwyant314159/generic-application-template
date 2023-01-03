#!/bin/bash
#
# Generate the compile_commands.json file for VSCode intellisense using the
# bear utility.

# Targets needed in the Makefile
CLEAN="clean"
FULL_BUILD="all"

# This script requires using the Makefile in the root of the example directory.
# This is equivalent to the parent directory of this script's location.
cd "$(dirname $0)/.." || exit 1

# Do a clean so we can run all the compile commands.
make "$CLEAN"

# Use the bear utility to record all the compiler commands used in the build.
bear --output .vscode/compile_commands.json -- make "$FULL_BUILD"