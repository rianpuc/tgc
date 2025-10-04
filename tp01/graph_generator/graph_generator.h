#ifndef __GRAPH_GENERATOR_H__
#define __GRAPH_GENERATOR_H__

typedef enum
{
    ESPARSO,
    MEDIO,
    DENSO
} Densidade;

typedef enum
{
    NAO_EULERIANO,
    SEMI_EULERIANO,
    EULERIANO
} Graph_Type;

typedef struct Node
{
    int id;
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
    Lista *v;  // lista de adjacencia normal
    Lista *rv; // lista auxiliar pra guardar os antecessores
    int *entrada;
    int *saida;
    int vc; // quantidade de vertices no grafo
    int arestas;
} Graph;

Graph createGraphEulerian(int vertices, Densidade d);
Graph createGraphSemiEulerian(int vertices, Densidade d);
Graph createGraphNotEulerian(int vertices, Densidade d);

void printDegrees(Graph *graph);

#endif