#!/bin/bash

strHelp="apply all patches"
source "`which gitToolsCommonCode.sh`"

function FUNCSBKPapplyPatch() { 
  strSBKP="$1"; 
  strFile="${strSBKP%.sbkp}"
  strPatch="${strFile}.patch"
  if ! patch --merge -p1 <"./$strPatch";then
    meld "$strFile" #TODO let conflicts be found when hitting Ctrl+K
  fi
};export -f FUNCSBKPapplyPatch; 

find ./ -iname "*.sbkp" -exec bash -c 'FUNCSBKPapplyPatch {}' \;
