# C++ port of Ruslan Spivaks "Let's Build a Simple Interpreter" tutorial.
#
# By Jaldhar H. Vyas <jaldhar@braincells.com>
# Copyright (C) 2017, Consolidated Braincells Inc. All rights reserved.
# "Do what thou wilt" shall be the whole of the license.

CXX=c++
CXXFLAGS=-std=c++14 -O2 -g -Wall -Wextra -Wcast-qual -Wformat=2
LDFLAGS=

.POSIX:

.cc.o:
	$(CXX) $(CXXFLAGS) -c -o $@ $^

all: calc1 calc2 calc3 calc4

calc1: calc1.o
	$(CXX) $(LDFLAGS) -o $@ $<

calc2: calc2.o
	$(CXX) $(LDFLAGS) -o $@ $<

calc3: calc3.o
	$(CXX) $(LDFLAGS) -o $@ $<

calc4: calc4.o
	$(CXX) $(LDFLAGS) -o $@ $<

clean:
	-rm calc1 calc2 calc3 calc4 *.o
