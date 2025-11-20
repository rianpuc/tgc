CC = g++
CFLAGS = -O3 -std=c++17

all: solver

solver: solver.cpp
	$(CC) $(CFLAGS) $^ -o $@ 
	
clean:
	rm -f adjlist *.o *.so