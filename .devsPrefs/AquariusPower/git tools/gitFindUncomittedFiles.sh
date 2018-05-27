#!/bin/bash

source "`which gitToolsCommonCode.sh`"

bSbkp=false
if [[ "${1-}" == "--sbkp" ]];then #help create .sbkp files
  shift
  #astrCmdAdd=(-exec cp -v '{}' '{}.sbkp' ';')
  bSbkp=true;
fi

strCurrentBranch="$(git branch |grep "[*]" |sed -r 's@..(.*)@\1@')"
declare -p strCurrentBranch

strRef="${1-}" #help [strRef] may be used instead of default branch file TODO this needs improvement
if [[ -z "$strRef" ]];then
  strRef="./.git/refs/heads/$strCurrentBranch"
fi
declare -p strRef

IFS=$'\n' read -d '' -r -a astrFileList < \
  <( \
    find ./ \
      -type f \
      -and '(' -iname "*.cpp" -or -iname "*.h" ')' \
      -and -cnewer "$strRef" \
  )
declare -p astrFileList

if [[ -z "${astrFileList[@]-}" ]];then echo "Nothing changed...";exit 0;fi

ls -ltr --time-style=full-iso "$strRef" "${astrFileList[@]}"

if $bSbkp;then
  for strFile in "${astrFileList[@]}";do
    cp -v "$strFile" "${strFile}.sbkp"
  done
fi
