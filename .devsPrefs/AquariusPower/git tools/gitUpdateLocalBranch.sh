#!/bin/bash

strHelp="easily update a local branch"
source "`which gitToolsCommonCode.sh`"

strBranch="${1-}";

if [[ -z "$strBranch" ]];then
  git branch
  echo "PROBLEM: a branch to update is required as parameter"
  exit 1
fi

git branch |egrep " $strBranch$"

FUNCexecEchoW git branch "${strBranch}_BKP_`date '+%Y%m%d_%H%M%S'`" "$strBranch" # create a backup branch before updating it!!!!

FUNCexecEchoW git checkout "$strBranch";

FUNCexecEchoW git fetch origin "$strBranch";

FUNCexecEchoW git pull origin "$strBranch";
