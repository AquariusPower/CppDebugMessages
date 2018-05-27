set -u #so when unset variables are expanded, gives fatal error

echo "HELP: ${strHelp-}"
egrep "[#]help" $0
echo "-------------------"
echo

pwd;if [[ ! -f ./.git/config ]];then echo "PROBLEM: not at valid VCS path";exit 1;fi

function FUNCexecEcho() { echo; echo " >>> EXEC: $@"; "$@"; }
function FUNCexecEchoW() { FUNCexecEcho "$@"; read -p ">>> press a key to continue..."; }
