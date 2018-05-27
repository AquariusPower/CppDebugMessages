#!/bin/bash

strHelp="trash VCS discarded/unused/useless/old/deprecated files git"
source "`which gitToolsCommonCode.sh`"

FUNCexecEcho find ./ \
  '(' -iname "*.rej" -o -iname "*.orig" -o -iname "*.sbkp" -o -iname "*.patch" ')' \
  -exec trash -v '{}' \;
