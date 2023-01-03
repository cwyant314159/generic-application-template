#!/bin/bash
#

# The programmer utility. This is only here if the tools is not on your path.
PROGRAMMER="avrdude"

# Use the Atmel ICE programmer to target a 328P controller. Add a little extra
# verbosity for good measure.
AVRDFLAGS="-c atmelice_isp -p m328p -v -v "

[[ -f $1 ]] || {
    echo "Cannot find file $1"
    exit 1
}

# Write and verify an Intel hex file to flash 
AVRDFLAGS="${AVRDFLAGS} -U flash:w:$1:i"

$PROGRAMMER $AVRDFLAGS
