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

#~ strRef="${1-}" # help [strRef] may be used instead of default branch file TODO this needs improvement
#~ if [[ -z "$strRef" ]];then
  #~ strRef="./.git/refs/heads/$strCurrentBranch"
#~ fi
#~ declare -p strRef

#TODO ? compare each against the files we get after reverting changes. create a new tmp branch to get them unchanged to compare? what is the less troublesome option? should just `git compare` each against unmodified ones! surely there is a command for that...
#TODO much easier: use some git command to find and list the changes instead of the `find` command. This would even be more portable between OSs

sedOnlyChangedFiles='s@^[+][+][+] ./(.*)@\1@'
IFS=$'\n' read -d '' -r -a astrFileList < <(git diff |grep "+++ ./" |sed -r "$sedOnlyChangedFiles")

#~ IFS=$'\n' read -d '' -r -a astrFileList < \
  #~ <( \
    #~ find ./ \
      #~ -type f \
      #~ -and '(' -iname "*.cpp" -or -iname "*.h" ')' \
      #~ -and -cnewer "$strRef" \
  #~ )
declare -p astrFileList

if [[ -z "${astrFileList[@]-}" ]];then echo "Nothing changed...";exit 0;fi

#ls -ltr --time-style=full-iso "$strRef" "${astrFileList[@]}"
ls -ltr --time-style=full-iso "${astrFileList[@]}"

if $bSbkp;then
  for strFile in "${astrFileList[@]}";do
    cp -v "$strFile" "${strFile}.sbkp"
  done
fi
