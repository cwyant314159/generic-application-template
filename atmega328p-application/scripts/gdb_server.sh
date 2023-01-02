#! /bin/sh
#
# Start an instance of openocd with the configuration file in the scripts
# directory. Any argument passed to this script is passed down to openocd.

SCRIPT="$(dirname $0)/openocd.cfg"
openocd -f "$SCRIPT" $@