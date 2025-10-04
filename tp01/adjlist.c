#include <stdio.h>
#include <stdlib.h>
#include "graph_generator/graph_generator.h"

typedef struct
{
    int TD;
    int TT;
    int pai;
} DFS;

typedef struct StackNode
{
    int id;
    struct StackNode *next;
} StackNode;

typedef struct
{
    StackNode *head;
} Stack;

Stack stack;
int tempo = 0;

void criarPilha()
{
    stack.head = malloc(sizeof(StackNode));
    stack.head = NULL;
}

int pilhaVazia()
{
    return stack.head == NULL;
}

void adicionarPilha(int item)
{
    StackNode *stacknode = malloc(sizeof(StackNode));
    stacknode->id = item;
    stacknode->next = stack.head;
    stack.head = stacknode;
}

int removerDaPilha()
{
    StackNode *prev = stack.head;
    stack.head = prev->next;
    int value = prev->id;
    free(prev);
    return value;
}

int verTopoDaPilha()
{
    return stack.head->id;
}

DFS *initializeDFSTable(int vertices)
{
    DFS *dfs = malloc(sizeof(DFS) * vertices);
    for (int i = 0; i < vertices; i++)
    {
        dfs[i].TD = 0;
        dfs[i].TT = 0;
        dfs[i].pai = -1;
    }
    return dfs;
}

void DFS_Stack(Graph *graph, DFS *dfs_table)
{
    for (int i = 0; i < graph->vc; i++)
    {
        if (dfs_table[i].TD == 0)
        {
            adicionarPilha(i);
            tempo++;
            dfs_table[i].TD = tempo;
            while (!pilhaVazia())
            {
                int u = verTopoDaPilha();
                Node *vizinhos = graph->v[u].head;
                Node *proximo_vizinho_nao_visitado = NULL;
                while (vizinhos != NULL)
                {
                    if (dfs_table[vizinhos->id].TD == 0)
                    {
                        // printf("{%d, %d} - nao visitada\n", u + 1, vizinhos->id + 1);
                        proximo_vizinho_nao_visitado = vizinhos;
                        break;
                    }
                    vizinhos = vizinhos->next;
                }
                if (proximo_vizinho_nao_visitado != NULL)
                {
                    int v = proximo_vizinho_nao_visitado->id;
                    // printf("vizinho n visitado: %d vai ser oproximo\n", v + 1);
                    dfs_table[v].pai = u;
                    tempo++;
                    dfs_table[v].TD = tempo;
                    adicionarPilha(v);
                    // printf("olhando a pilha: %d\n", verTopoDaPilha() + 1);
                }
                else
                {
                    // printf("%d nao tem nada vou remover\n", u + 1);
                    tempo++;
                    dfs_table[u].TT = tempo;
                    u = removerDaPilha();
                }
            }
        }
    }
}

void printGraph(Graph *graph) // so para debugar
{
    for (int i = 0; i < graph->vc; i++)
    {
        Node *ptr = graph->v[i].head;
        printf("Vertice: %d, Vizinhos:\n", i);
        while (ptr != NULL)
        {
            printf("%d ", ptr->id);
            ptr = ptr->next;
        }
        printf("\n");
    }
    printf("\n");
}

void printDFS(DFS *dfs_table, int vertices)
{
    for (int i = 0; i < vertices; i++)
    {
        printf("{%d: TD - %d; TT - %d; pai - %d}\n", i + 1, dfs_table[i].TD, dfs_table[i].TT, dfs_table[i].pai + 1);
    }
}

int main()
{
    Graph graph = createGraphNotEulerian(5, ESPARSO);
    printGraph(&graph);
    printDegrees(&graph);
    return 0;
}