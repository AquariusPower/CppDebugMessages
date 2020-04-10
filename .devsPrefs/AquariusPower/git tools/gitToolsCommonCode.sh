set -u #so when unset variables are expanded, gives fatal error

echo "HELP: ${strHelp-}"
egrep "[#]help" $0
echo "-------------------"
echo

pwd;if [[ ! -f ./.git/config ]];then echo "PROBLEM: not at valid VCS path";exit 1;fi

function FUNCexecEcho() { echo; echo " >>> EXEC: $@"; "$@"; return $?; }
function FUNCexecEchoW() { FUNCexecEcho "$@"; local nRet=$?; if((nRet!=0));then echo "error $nRet";return $nRet;fi; read -p ">>> press ENTER/RETURN to continue..."; }
