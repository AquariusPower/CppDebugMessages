#!/bin/bash

strHelp="create a new branch using another user's branch of that project"
source "`which gitToolsCommonCode.sh`"

strUser="$1";shift #help <strUser> user name on github
strProject="$1";shift #help <strProject> the project id/name
strBranch="$1";shift #help <strBranch> user branch on github
: ${strBaseBranch:=master};shift #help [strBaseBranch] base local branch to start from

FUNCexecEchoW git branch

if ! FUNCexecEchoW git checkout -b "${strUser}-${strBranch}" master;then
  if ! FUNCexecEchoW git checkout "${strUser}-${strBranch}";then
    exit 1
  else
    FUNCexecEchoW git branch
  fi
fi

FUNCexecEcho git pull "https://github.com/$strUser/${strProject}.git" "${strBranch}"
