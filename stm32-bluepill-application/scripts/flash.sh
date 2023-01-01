#!/bin/bash
#

programmer="${STM32CUBEPROGRAMMER_ROOT}/bin/STM32_Programmer_CLI"

[[ -f $programmer ]] || {
    echo "Cannot find $(basename $programmer) at $(dirname $programmer)"
    exit 1
}

[[ -f $1 ]] || {
    echo "Cannot find file $1"
    exit 1
}

$programmer -c port=swd -w $1 -rst
