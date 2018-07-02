#!/bin/bash

strHelp="apply all patches"
source "`which gitToolsCommonCode.sh`"

function FUNCSBKPapplyPatch() { 
  strFile="$1"; 
  strPatch="./${strFile%.sbkp}.patch"
  patch --merge -p1 <"$strPatch"
};export -f FUNCSBKPapplyPatch; 

find ./ -iname "*.sbkp" -exec bash -c 'FUNCSBKPapplyPatch {}' \;
