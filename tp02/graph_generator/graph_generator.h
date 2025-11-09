#ifndef __GRAPH_GENERATOR_H__
#define __GRAPH_GENERATOR_H__

typedef struct Node
{
    int id;
    int weight;
    struct Node *next;
    struct Node *prev;
} Node;

typedef struct
{
    Node *head;
    Node *tail;
} Lista;

typedef struct
{
    Lista *v; // lista de adjacencia normal
    int **distancias;
    int *grau;
    int vc; // quantidade de vertices no grafo
    int arestas;
    int k;
} Graph;

Graph importGraph(const char *filename);
#endif