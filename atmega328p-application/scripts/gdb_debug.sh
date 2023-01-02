#! /bin/sh
#
# Start an instance of arm-none-eabi-gdb with the GDB script in the scripts
# directory. Any argument passed to this script is passed down to GDB.

SCRIPT="$(dirname $0)/debug.gdb"
arm-none-eabi-gdb --command="$SCRIPT" $@