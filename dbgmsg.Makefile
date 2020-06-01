.POSIX:
.SUFFIXES: .cpp
CXX      = c++
CXXFLAGS = -std=c++17 -Wall -Wextra -O3 -g3 -ggdb3 -pthread -Wdangling-else -Wparentheses -Wreturn-type -Wno-format-security -fno-omit-frame-pointer -DUNIX -DDBGMSG -DDBGMSG_SELF_TEST -DDBGMSG_ALLOWOE

all: dbgmsg

dbgmsg: dbgmsg.o
	$(CXX) $(LDFLAGS) -o $@ dbgmsg.o $(LDLIBS) -lpthread

dbgmsg.o: dbgmsg.cpp

clean:
	trash dbgmsg dbgmsg.o

.cc.o:
	$(CXX) -c $(CXXFLAGS) -o $@ -pthread $<
