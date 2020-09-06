CC=g++
CXX=g++
CXXFLAGS=-std=c++11 -Wall -Wextra -pedantic -O3
CFLAGS=-std=c++11 -Wall -Wextra -pedantic -O3

TARGETS = bms2A bms2B 

all: $(TARGETS)

bms2A: bms2A.o galois.o berlekamp.o crcgen.o rs.o
	$(CXX) -o $@ $^

bms2B: bms2B.o galois.o berlekamp.o crcgen.o rs.o
	$(CXX) -o $@ $^

clean:
	rm -f *.o $(TARGETS) *.err *.out *.ok

