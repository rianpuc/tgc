CC = gcc
CFLAGS = -Wall -Wextra -std=c99
OBJS = graph_generator.o

all: adjlist

adjlist: adjlist.c $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ -lm 

graph_generator.o: graph_generator/graph_generator.c graph_generator/graph_generator.h
	$(CC) $(CFLAGS) -c $< -o $@ # Use $< para garantir apenas o arquivo .c

clean:
	rm -f adjlist *.o *.so