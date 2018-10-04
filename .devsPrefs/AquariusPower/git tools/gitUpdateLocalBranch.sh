#!/bin/bash

strHelp="easily update a local branch"
source "`which gitToolsCommonCode.sh`"

strBranch="$1";

git branch |egrep " $strBranch$"

FUNCexecEchoW git branch "${strBranch}_BKP_`date '+%Y%m%d_%H%M%S'`" "$strBranch" # create a backup branch before updating it!!!!

FUNCexecEchoW git fetch;

FUNCexecEchoW git checkout "$strBranch";

FUNCexecEchoW git pull origin "$strBranch";
