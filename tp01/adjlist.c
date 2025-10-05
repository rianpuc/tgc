#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "graph_generator/graph_generator.h"
#define min(a, b) (((a) < (b)) ? (a) : (b))

typedef struct
{
    int TD;
    int TT;
    int LOW;
    int pai;
} DFS;

typedef struct
{
    int u;
    int v;
} Ponte;

typedef struct StackNode
{
    int id;
    struct StackNode *next;
} StackNode;

typedef struct
{
    StackNode *head;
} Stack;

Ponte *listaPontes;
int tempo = 0;
int bridge_count = 0;

void printGraph(Graph *graph);

Stack criarPilha()
{
    Stack stack;
    stack.head = NULL;
    return stack;
}

int pilhaVazia(Stack *stack)
{
    return stack->head == NULL;
}

void adicionarPilha(Stack *stack, int item)
{
    StackNode *stacknode = malloc(sizeof(StackNode));
    stacknode->id = item;
    stacknode->next = stack->head;
    stack->head = stacknode;
}

int removerDaPilha(Stack *stack)
{
    StackNode *prev = stack->head;
    stack->head = prev->next;
    int value = prev->id;
    free(prev);
    return value;
}

int verTopoDaPilha(Stack *stack)
{
    return stack->head->id;
}

void freeStack(Stack *stack)
{
    StackNode *current = stack->head;
    StackNode *next_node;
    while (current != NULL)
    {
        next_node = current->next;
        free(current);
        current = next_node;
    }
    stack->head = NULL;
}

void freeGraph(Graph *graph)
{
    if (graph == NULL)
        return;

    for (int i = 0; i < graph->vc; i++)
    {
        Node *current = graph->v[i].head;
        Node *next_node;
        while (current != NULL)
        {
            next_node = current->next;
            free(current);
            current = next_node;
        }
    }

    if (graph->v != NULL)
    {
        free(graph->v);
    }
    if (graph->grau != NULL)
    {
        free(graph->grau);
    }
}

void adicionarPonte(int p, int u)
{
    Ponte nova;
    nova.u = p;
    nova.v = u;
    listaPontes[bridge_count++] = nova;
}

DFS *initializeDFSTable(int vertices)
{
    DFS *dfs = malloc(sizeof(DFS) * vertices);
    for (int i = 0; i < vertices; i++)
    {
        dfs[i].TD = 0;
        dfs[i].TT = 0;
        dfs[i].LOW = 0;
        dfs[i].pai = -1;
    }
    return dfs;
}

void DFS_Stack(Graph *graph, DFS *dfs_table)
{
    Stack stack = criarPilha();
    for (int i = 0; i < graph->vc; i++)
    {
        if (dfs_table[i].TD == 0)
        {
            adicionarPilha(&stack, i);
            tempo++;
            dfs_table[i].TD = tempo;
            dfs_table[i].LOW = tempo;
            while (!pilhaVazia(&stack))
            {
                int u = verTopoDaPilha(&stack);
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
                    else if (vizinhos->id != dfs_table[u].pai)
                    {
                        dfs_table[u].LOW = min(dfs_table[u].LOW, dfs_table[vizinhos->id].TD);
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
                    dfs_table[v].LOW = tempo;
                    adicionarPilha(&stack, v);
                    // printf("olhando a pilha: %d\n", verTopoDaPilha() + 1);
                }
                else
                {
                    if (dfs_table[u].pai != -1)
                    {
                        int p = dfs_table[u].pai;
                        if (dfs_table[u].LOW > dfs_table[p].TD)
                        {
                            adicionarPonte(p, u);
                        }
                        dfs_table[p].LOW = min(dfs_table[p].LOW, dfs_table[u].LOW);
                    }
                    // printf("%d nao tem nada vou remover\n", u + 1);
                    tempo++;
                    dfs_table[u].TT = tempo;
                    u = removerDaPilha(&stack);
                }
            }
        }
    }
    freeStack(&stack);
}

int DFS_Count_Visited(Graph *graph, int comeco, int ignorar_v, int ignorar_w)
{
    Stack stack = criarPilha();
    bool *visited = malloc(sizeof(bool) * graph->vc);
    for (int i = 0; i < graph->vc; i++)
    {
        visited[i] = false;
    }
    int count = 0;
    adicionarPilha(&stack, comeco);
    visited[comeco] = true;
    count++;
    while (!pilhaVazia(&stack))
    {
        int u = removerDaPilha(&stack);
        for (Node *vizinho = graph->v[u].head; vizinho != NULL; vizinho = vizinho->next)
        {
            int v = vizinho->id;
            if ((u == ignorar_v && v == ignorar_w) || (u == ignorar_w && v == ignorar_v))
            {
                continue;
            }
            if (!visited[v])
            {
                visited[v] = true;
                count++;
                adicionarPilha(&stack, v);
            }
        }
    }
    free(visited);
    return count;
}

int countActiveVertices(Graph *graph)
{
    int active_count = 0;
    for (int i = 0; i < graph->vc; i++)
    {
        if (graph->grau[i] > 0)
        {
            active_count++;
        }
    }
    return active_count;
}

int getStartingVertex(Graph *graph)
{
    if (graph->odd_count == 0)
    {
        return 0;
    }
    else if (graph->odd_count == 2)
    {
        return graph->odd_vertices_pool[0];
    }
    else
    {
        return -1;
    }
}

Ponte *pontesTarjan(Graph *graph)
{
    Ponte *pontes = malloc(sizeof(Ponte) * (graph->vc - 1));
    listaPontes = pontes;
    bridge_count = 0;
    DFS *dfs = initializeDFSTable(graph->vc);
    tempo = 0;
    DFS_Stack(graph, dfs);
    return pontes;
}

Ponte *pontesNaive(Graph *graph)
{
    int active_vertices = countActiveVertices(graph);
    int vertices = graph->vc;
    Ponte *pontes = malloc(sizeof(Ponte) * (vertices));
    bridge_count = 0;
    listaPontes = pontes;
    for (int i = 0; i < vertices; i++)
    {
        for (Node *vizinho = graph->v[i].head; vizinho != NULL; vizinho = vizinho->next)
        {
            int v = vizinho->id;
            if (i > v)
                continue;
            int visited_count = DFS_Count_Visited(graph, i, i, v);
            if (visited_count < active_vertices)
            {
                adicionarPonte(i, v);
            }
        }
    }
    return pontes;
}

bool isBridgeTarjan(Graph *current_graph, int u, int v)
{
    Ponte *pontes = pontesTarjan(current_graph);
    int quantidade_pontes = bridge_count;
    bool found = false;
    for (int i = 0; i < quantidade_pontes; i++)
    {
        if ((pontes[i].u == u && pontes[i].v == v) || (pontes[i].u == v && pontes[i].v == u))
        {
            found = true;
            break;
        }
    }
    free(pontes);
    return found;
}

bool isBridgeNaive(Graph *current_graph, int u, int v)
{
    Ponte *pontes = pontesNaive(current_graph);
    int quantidade_pontes = bridge_count;
    bool found = false;
    for (int i = 0; i < quantidade_pontes; i++)
    {
        if ((pontes[i].u == u && pontes[i].v == v) || (pontes[i].u == v && pontes[i].v == u))
        {
            found = true;
            break;
        }
    }
    free(pontes);
    return found;
}

int *fleuryTarjan(Graph *source_graph, int *path_length)
{

    int start_v = getStartingVertex(source_graph);
    if (start_v == -1)
    {
        *path_length = 0;
        return NULL;
    }
    Graph g_copy = copyGraph(source_graph);

    int *path = malloc(sizeof(int) * (source_graph->arestas + 1));
    int current_path_length = 0;
    int current_v = start_v;
    bridge_count = 0;

    while (g_copy.arestas > 0)
    {
        path[current_path_length++] = current_v;
        int next_v = -1;
        int bridge_v = -1;
        Node *neighbor = g_copy.v[current_v].head;

        while (neighbor != NULL)
        {
            int v = neighbor->id;
            bool is_bridge = isBridgeTarjan(&g_copy, current_v, v);
            if (!is_bridge)
            {
                next_v = v;
                break;
            }
            if (g_copy.grau[current_v] == 1)
            {
                bridge_v = v;
            }
            neighbor = neighbor->next;
        }

        if (next_v == -1)
        {
            if (bridge_v != -1)
            {
                next_v = bridge_v;
            }
            else
            {

                fprintf(stderr, "Erro de Travessia: Grafo desconectado prematuramente.\n");
                break;
            }
        }
        removeEdgeLocal(&g_copy, current_v, next_v);
        current_v = next_v;
    }

    path[current_path_length] = current_v;
    *path_length = current_path_length + 1;
    freeGraph(&g_copy);
    return path;
}

int *fleuryNaive(Graph *source_graph, int *path_length)
{

    int start_v = getStartingVertex(source_graph);
    if (start_v == -1)
    {
        *path_length = 0;
        return NULL;
    }
    Graph g_copy = copyGraph(source_graph);

    int *path = malloc(sizeof(int) * (source_graph->arestas + 1));
    int current_path_length = 0;
    int current_v = start_v;

    while (g_copy.arestas > 0)
    {
        path[current_path_length++] = current_v;
        int next_v = -1;
        int bridge_v = -1;
        Node *neighbor = g_copy.v[current_v].head;

        while (neighbor != NULL)
        {
            int v = neighbor->id;
            bool is_bridge = isBridgeNaive(&g_copy, current_v, v);
            if (!is_bridge)
            {
                next_v = v;
                break;
            }
            if (g_copy.grau[current_v] == 1)
            {
                bridge_v = v;
            }
            neighbor = neighbor->next;
        }
        if (next_v == -1)
        {
            if (bridge_v != -1)
            {
                next_v = bridge_v;
            }
            else
            {

                fprintf(stderr, "Erro de Travessia: Grafo desconectado prematuramente.\n");
                break;
            }
        }
        removeEdgeLocal(&g_copy, current_v, next_v);
        current_v = next_v;
    }
    path[current_path_length] = current_v;
    *path_length = current_path_length + 1;
    freeGraph(&g_copy);
    return path;
}

void printGraph(Graph *graph) // so para debugar
{
    printf("---Grafo---\n\tVertices: %d\n\tArestas: %d\n\tVertices Impares: %d\n", graph->vc, graph->arestas, graph->odd_count);
    for (int i = 0; i < graph->vc; i++)
    {
        Node *ptr = graph->v[i].head;
        printf("Vertice: %d, Vizinhos:\n", i + 1);
        while (ptr != NULL)
        {
            printf("%d ", ptr->id + 1);
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
        printf("{%d: TD - %d; TT - %d; LOW - %d; pai - %d}\n", i + 1, dfs_table[i].TD, dfs_table[i].TT, dfs_table[i].LOW, dfs_table[i].pai + 1);
    }
}

void printPontes(Ponte *pontes, int quantidade)
{
    for (int i = 0; i < quantidade; i++)
    {
        printf("Ponte: {%d, %d}\n", pontes[i].u, pontes[i].v);
    }
}

int main()
{
    srand(time(NULL));
    Graph graph = createGraphEulerian(100, ESPARSO);
    // DFS *dfs = initializeDFSTable(graph.vc);
    tempo = 0;
    // printGraph(&graph);
    // DFS_Stack(&graph, dfs);
    // printDegrees(&graph);
    // printDFS(dfs, graph.vc);
    int path_naive_count = 0;
    int *path_naive = fleuryNaive(&graph, &path_naive_count);
    for (int i = 0; i < path_naive_count; i++)
    {
        if (i < path_naive_count - 1)
            printf("%d -> ", path_naive[i]);
        else
            printf("%d", path_naive[i]);
    }
    printf("\n");
    int path_tarjan_count = 0;
    int *path_tarjan = fleuryTarjan(&graph, &path_tarjan_count);
    for (int i = 0; i < path_tarjan_count; i++)
    {
        if (i < path_tarjan_count - 1)
            printf("%d -> ", path_tarjan[i]);
        else
            printf("%d", path_tarjan[i]);
    }
    return 0;
}