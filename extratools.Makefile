.POSIX:
.SUFFIXES: .cpp
CXX      = c++
CXXFLAGS = -std=c++17 -Wall -Wextra -O3 -g3 -pthread

all: extratools

extratools: extratools.o
	$(CXX) $(LDFLAGS) -o $@ extratools.o $(LDLIBS) -lpthread

extratools.o: extratools.cpp

clean:
	trash extratools extratools.o

.cc.o:
	$(CXX) -c $(CXXFLAGS) -o $@ -pthread $<
