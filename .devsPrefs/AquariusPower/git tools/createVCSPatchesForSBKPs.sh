#!/bin/bash

pwd;if [[ ! -f ./.git/config ]];then echoc -p "not at valid VCS path";exit 1;fi

echo "create patches for all sbkp files"

function FUNCSBKPcreatePatch() { 
  strFile="$1"; 
  strPatch="./${strFile%.sbkp}.patch"
  diff -Naru "./${strFile%.sbkp}" "./${strFile}" >"$strPatch"; 
  ls -l "$strPatch"
};export -f FUNCSBKPcreatePatch; 

find ./ -iname "*.sbkp" -exec bash -c 'FUNCSBKPcreatePatch {}' \;
