#!/bin/bash

strHelp="easily update a local branch"
source "`which gitToolsCommonCode.sh`"

strBranch="$1";

git branch |egrep " $strBranch$"

FUNCexecEchoW git fetch;

FUNCexecEchoW git checkout "$strBranch";

FUNCexecEchoW git pull origin "$strBranch";
