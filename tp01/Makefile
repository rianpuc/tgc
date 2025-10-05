CC = gcc
#CFLAGS = -Wall -Wextra -std=c99 -O3 -march=native -ffast-math -m64
CFLAGS = -Wall -Wextra -std=c99 -O3 -march=native -ffast-math
OBJS = graph_generator.o

all: adjlist

adjlist: adjlist.c $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

graph_generator.o: graph_generator/graph_generator.c graph_generator/graph_generator.h
	$(CC) $(CFLAGS) -c graph_generator/graph_generator.c -o graph_generator.o

clean:
	rm -f adjlist *.o *.so


