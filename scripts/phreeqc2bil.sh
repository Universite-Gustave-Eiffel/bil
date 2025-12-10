#!/bin/sh

awk -v data=$1 -v phase=$2 --re-interval -f ${BIL}/scripts/phreeqc2bil.awk $1
