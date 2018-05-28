#!/bin/bash

strHelp="create a new branch using another user's branch of that project"
source "`which gitToolsCommonCode.sh`"

strUser="$1";shift #help <strUser> user name on github
strProject="$1";shift #help <strProject> the project id/name
strBranch="$1";shift #help <strBranch> user branch on github
: ${strBaseBranch:=master};shift #help [strBaseBranch] base local branch to start from

FUNCexecEchoW git branch

FUNCexecEchoW git checkout -b "${strUser}-${strBranch}" master

FUNCexecEcho git pull "https://github.com/$strUser/${strProject}.git" "${strBranch}"
