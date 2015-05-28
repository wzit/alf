
CXX 		= clang++

CXXFLAGS	= -std=c++11 -Wall -Wpedantic -Werror

REPEAT		= 10
PRES		= 10

all:		alf.h alf_test.cpp
			$(CXX) alf_test.cpp $(CXXFLAGS) -o alf_test

debug:		CXXFLAGS += -O0 -g
debug:		all

release:	CXX = g++
release:	CXXFLAGS += -O3 -DNDEBUG
release:	all

gcc:		CXX = g++
gcc:		all

run:
			./run.sh $(REPEAT)