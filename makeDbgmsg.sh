#!/bin/bash
make -f dbgmsg.Makefile clean;
if make -f dbgmsg.Makefile;then 
	./dbgmsg
	cat ~/.dbgmsg.dbgmsg.log;
fi
