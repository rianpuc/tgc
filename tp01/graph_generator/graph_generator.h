#ifndef __GRAPH_GENERATOR_H__
#define __GRAPH_GENERATOR_H__
#define HASH_TABLE_SIZE (4 * 1000000)
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

typedef struct HashNode
{
    int u;
    int v;
    struct HashNode *next;
} HashNode;

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
    Lista *v; // lista de adjacencia normal
    int *grau;
    int vc; // quantidade de vertices no grafo
    int arestas;
    int *odd_vertices_pool;
    int odd_count;
} Graph;

extern HashNode *edge_hash_table[HASH_TABLE_SIZE];

Graph createGraphConexo(int vertices, Densidade d);
Graph createGraphEulerian(int vertices, Densidade d);
Graph createGraphSemiEulerian(int vertices, Densidade d);
Graph createGraphNotEulerian(int vertices, Densidade d);
Graph copyGraph(Graph *source);
void removeEdgeLocal(Graph *graph, int u, int v);
Graph createHaltereGraph();
void cleanupMemory();
void resetGlobalState();
void printDegrees(Graph *graph);

#endif