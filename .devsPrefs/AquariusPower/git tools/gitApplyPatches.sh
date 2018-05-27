#!/bin/bash

pwd;if [[ ! -f ./.git/config ]];then echoc -p "not at valid VCS path";exit 1;fi

function FUNCSBKPapplyPatch() { 
  strFile="$1"; 
  strPatch="./${strFile%.sbkp}.patch"
  patch -p1 <"$strPatch"
};export -f FUNCSBKPapplyPatch; 

find ./ -iname "*.sbkp" -exec bash -c 'FUNCSBKPapplyPatch {}' \;
