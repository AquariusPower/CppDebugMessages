#!/bin/bash

echo "WIP!!! "

echo "showing them"
egrep --color "(DBG[A-Z]*|DBG[^(]*[(][^)]*[)]);" * -rnI --include="*.h" --include="*.cpp"

#TODO cleanup with sed
#TODO really do it? it is not necessarily good as in: if DBGMSG is not defined all that code will not be compiled and effort was put on preparing it...
