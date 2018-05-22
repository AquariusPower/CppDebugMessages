#!/bin/bash

source <(secinit)

pwd;if [[ ! -f ./.git/config ]];then echoc -p "not at valid VCS path";exit 1;fi

echo "trash VCS discarded/unused/useless/old/deprecated files git"

SECFUNCexecA -ce find ./ \
  '(' -iname "*.rej" -o -iname "*.orig" -o -iname "*.sbkp" -o -iname "*.patch" ')' \
  -exec trash -v '{}' \;
