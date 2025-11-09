CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g -O2 $(INCLUDES)
INCLUDES = -Ialgorithms -Igraph_generator
LDFLAGS = -lm
EXEC = main
OBJS = main.o \
	aproximado.o \
	exato.o \
	graph_generator.o
HDR_ALGO = algorithms/algorithms.h
HDR_GRAPH = graph_generator/graph_generator.h

ifeq ($(OS),Windows_NT)
	RM = del /F /Q
	TARGET_EXEC = $(EXEC).exe
	TARGET_OBJS = $(OBJS)
	EXTRA_CLEAN = $(EXEC)
	RM_ERR = 2> NUL
else
	RM = rm -f
	TARGET_EXEC = $(EXEC)
	TARGET_OBJS = $(OBJS)
	EXTRA_CLEAN =
	RM_ERR = 
endif

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) -Wl,--stack,32194304
	-$(RM) $(TARGET_OBJS) $(RM_ERR)

main.o: main.c $(HDR_ALGO) $(HDR_GRAPH)
	$(CC) $(CFLAGS) -c main.c -o main.o

aproximado.o: algorithms/aproximado.c $(HDR_ALGO) $(HDR_GRAPH)
	$(CC) $(CFLAGS) -c algorithms/aproximado.c -o aproximado.o

exato.o: algorithms/exato.c $(HDR_ALGO) $(HDR_GRAPH)
	$(CC) $(CFLAGS) -c algorithms/exato.c -o exato.o

graph_generator.o: graph_generator/graph_generator.c $(HDR_GRAPH)
	$(CC) $(CFLAGS) -c graph_generator/graph_generator.c -o graph_generator.o

clean:
	-$(RM) $(TARGET_OBJS)
	-$(RM) $(TARGET_EXEC)
	-$(RM) $(EXTRA_CLEAN)

.PHONY: all clean