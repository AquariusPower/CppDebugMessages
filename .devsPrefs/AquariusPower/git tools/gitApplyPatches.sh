#!/bin/bash

strHelp="create a new branch using another user's branch of that project"
source "`which gitToolsCommonCode.sh`"

function FUNCSBKPapplyPatch() { 
  strFile="$1"; 
  strPatch="./${strFile%.sbkp}.patch"
  patch -p1 <"$strPatch"
};export -f FUNCSBKPapplyPatch; 

find ./ -iname "*.sbkp" -exec bash -c 'FUNCSBKPapplyPatch {}' \;
