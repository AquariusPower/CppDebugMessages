#!/bin/bash

strHelp="create patches for all sbkp files"
source "`which gitToolsCommonCode.sh`"

function FUNCSBKPcreatePatch() { 
  strFile="$1"; 
  strPatch="./${strFile%.sbkp}.patch"
  diff -Naru "./${strFile%.sbkp}" "./${strFile}" >"$strPatch"; #TODO try `git diff --diff-algorithm=...` (see `git help diff`), may give better results!
  ls -l "$strPatch"
};export -f FUNCSBKPcreatePatch; 

find ./ -iname "*.sbkp" -exec bash -c 'FUNCSBKPcreatePatch {}' \;
