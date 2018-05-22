#!/bin/bash

source <(secinit)

pwd;if [[ ! -f ./.git/config ]];then echoc -p "not at valid VCS path";exit 1;fi

astrCmdAdd=()
if [[ "${1-}" == "--sbkp" ]];then
  shift
  astrCmdAdd=(-exec cp -v '{}' '{}.sbkp' ';')
fi

strCurrentBranch="$(git branch |grep "[*]" |sed -r 's@..(.*)@\1@')"

SECFUNCexecA -ce find ./ \
  -type f \
  -and '(' -iname "*.cpp" -or -iname "*.h" ')' \
  -and -cnewer "./.git/refs/remotes/origin/$strCurrentBranch" \
  ${astrCmdAdd[@]-}
