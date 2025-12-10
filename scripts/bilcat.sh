#!/bin/sh

for file in $@
  do cat -- "${file}"; printf "\n"; done
