#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include "graph_generator/graph_generator.h"
#define min(a, b) (((a) < (b)) ? (a) : (b))

typedef struct
{
    int TD;
    int TT;
    int LOW;
    int pai;
} DFS;

typedef struct DFS_State
{
    int v;
    int pai;
    Node *atual;
    bool visitado;
    bool processando;
    int filho_retorno;
    struct DFS_State *next;
} DFS_State;

typedef struct
{
    int u;
    int v;
} Ponte;

typedef struct
{
    Ponte *lista;
    int count;
} Pontes;

typedef struct
{
    DFS_State *head;
} Stack;

typedef struct SimpleStackNode
{
    int id;
    struct SimpleStackNode *next;
} SimpleStackNode;

typedef struct
{
    SimpleStackNode *head;
} SimpleStack;

int tempo = 0;

void printGraph(Graph *graph);

Stack criarPilha()
{
    Stack stack;
    stack.head = NULL;
    return stack;
}

SimpleStack criarPilhaSimples()
{
    SimpleStack stack;
    stack.head = NULL;
    return stack;
}

int pilhaVazia(Stack *stack)
{
    return stack->head == NULL;
}

int pilhaVaziaSimples(SimpleStack *stack)
{
    return stack->head == NULL;
}

void adicionarPilha(Stack *stack, DFS_State item)
{
    DFS_State *estado_node = malloc(sizeof(DFS_State));
    *estado_node = item;
    estado_node->next = stack->head;
    stack->head = estado_node;
}

void adicionarPilhaSimples(SimpleStack *stack, int item)
{
    SimpleStackNode *stacknode = malloc(sizeof(SimpleStackNode));
    stacknode->id = item;
    stacknode->next = stack->head;
    stack->head = stacknode;
}

void removerDaPilha(Stack *stack)
{
    DFS_State *prev = stack->head;
    stack->head = prev->next;
    free(prev);
}

int removerDaPilhaSimples(SimpleStack *stack)
{
    SimpleStackNode *prev = stack->head;
    stack->head = prev->next;
    int value = prev->id;
    free(prev);
    return value;
}

DFS_State *verTopoDaPilha(Stack *stack)
{
    return stack->head;
}

void freeStack(Stack *stack)
{
    DFS_State *current = stack->head;
    DFS_State *next_node;
    while (current != NULL)
    {
        next_node = current->next;
        free(current);
        current = next_node;
    }
    stack->head = NULL;
}

void freeStackSimples(SimpleStack *stack)
{
    SimpleStackNode *current = stack->head;
    SimpleStackNode *next_node;
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

void resetDFSTable(DFS *dfs_table, int vertices)
{
    size_t size = sizeof(DFS) * vertices;
    memset(dfs_table, 0, size);
}

void adicionarPonte(Pontes *pontes, int p, int u)
{
    Ponte nova;
    nova.u = p;
    nova.v = u;
    pontes->lista[pontes->count++] = nova;
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

void DFS_Stack(Graph *graph, DFS *dfs_table, Pontes *pontes)
{
    Stack stack = criarPilha();
    for (int i = 0; i < graph->vc; i++)
    {
        if (dfs_table[i].TD == 0)
        {
            DFS_State estado = {i, -1, graph->v[i].head, false, false, -1, NULL};
            adicionarPilha(&stack, estado);

            while (!pilhaVazia(&stack))
            {
                DFS_State *estado = verTopoDaPilha(&stack);
                int u = estado->v;
                if (!estado->visitado)
                {
                    tempo++;
                    dfs_table[u].TD = dfs_table[u].LOW = tempo;
                    dfs_table[u].pai = estado->pai;
                    estado->visitado = true;
                }
                if (estado->processando)
                {
                    int w = estado->filho_retorno;
                    dfs_table[u].LOW = min(dfs_table[u].LOW, dfs_table[w].LOW);
                    if (dfs_table[w].LOW > dfs_table[u].TD)
                    {
                        adicionarPonte(pontes, u, w);
                    }
                    estado->processando = false;
                }
                if (estado->atual != NULL)
                {
                    Node *atual_vizinho = estado->atual;
                    int w = atual_vizinho->id;
                    estado->atual = atual_vizinho->next;
                    if (dfs_table[w].TD == 0)
                    {
                        estado->processando = true;
                        estado->filho_retorno = w;
                        DFS_State novo = {w, u, graph->v[w].head, false, false, -1, NULL};
                        adicionarPilha(&stack, novo);
                    }
                    else if (w != dfs_table[u].pai)
                    {
                        dfs_table[u].LOW = min(dfs_table[u].LOW, dfs_table[w].TD);
                    }
                }
                else
                {
                    removerDaPilha(&stack);
                }
            }
        }
    }
    freeStack(&stack);
}

int DFS_Count_Visited(Graph *graph, int comeco, int ignorar_v, int ignorar_w)
{
    SimpleStack stack = criarPilhaSimples();
    bool *visited = malloc(sizeof(bool) * graph->vc);
    for (int i = 0; i < graph->vc; i++)
    {
        visited[i] = false;
    }
    int count = 0;
    adicionarPilhaSimples(&stack, comeco);
    visited[comeco] = true;
    count++;
    while (!pilhaVaziaSimples(&stack))
    {
        int u = removerDaPilhaSimples(&stack);
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
                adicionarPilhaSimples(&stack, v);
            }
        }
    }
    free(visited);
    freeStackSimples(&stack);
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

Pontes pontesTarjan(Graph *graph, DFS *dfs)
{
    Pontes pontes;
    pontes.count = 0;
    pontes.lista = malloc(sizeof(Ponte) * (graph->vc - 1));
    resetDFSTable(dfs, graph->vc);
    tempo = 0;
    DFS_Stack(graph, dfs, &pontes);
    return pontes;
}

Pontes pontesNaive(Graph *graph)
{
    int active_vertices = countActiveVertices(graph);
    int vertices = graph->vc;
    Pontes pontes;
    pontes.count = 0;
    pontes.lista = malloc(sizeof(Ponte) * (graph->vc - 1));

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
                adicionarPonte(&pontes, i, v);
            }
        }
    }
    return pontes;
}

// bool isBridgeTarjan(Pontes *pontes, int u, int v)
// {
//     bool found = false;
//     for (int i = 0; i < pontes->count; i++)
//     {
//         if ((pontes->lista[i].u == u && pontes->lista[i].v == v) || (pontes->lista[i].u == v && pontes->lista[i].v == u))
//         {
//             found = true;
//             break;
//         }
//     }
//     return found;
// }

bool isBridgeTarjan(DFS *dfs_table, int u, int v)
{
    // A lógica de ponte deve ser: Se (u,v) é uma aresta de árvore e LOW[v] > TD[u].

    // 1. Identificar Pai e Filho na Árvore DFS
    int p, child;

    // Tarjan requer que a aresta seja da ÁRVORE DFS.
    // Como você está checando todos os vizinhos, precisamos ver quem é o pai de quem.
    if (dfs_table[u].TD < dfs_table[v].TD)
    { // u é o pai (desceu para v)
        p = u;
        child = v;
    }
    else
    { // v é o pai (desceu para u)
        p = v;
        child = u;
    }

    // 2. Critério Mágico (LOW > TD)
    // Se a aresta {u, v} é uma aresta de árvore, e a subárvore do filho não volta.
    if (dfs_table[v].pai == u || dfs_table[u].pai == v)
    { // É uma aresta da Árvore DFS
        if (dfs_table[child].LOW > dfs_table[p].TD)
        {
            return true;
        }
    }

    // Para todas as arestas de retorno e arestas que não são pontes
    return false;
}

bool isBridgeNaive(Graph *current_graph, int u, int v)
{
    Pontes resultado = pontesNaive(current_graph);
    bool found = false;
    for (int i = 0; i < resultado.count; i++)
    {
        if ((resultado.lista[i].u == u && resultado.lista[i].v == v) || (resultado.lista[i].u == v && resultado.lista[i].v == u))
        {
            found = true;
            break;
        }
    }
    free(resultado.lista);
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
    DFS *tabela_dfs = initializeDFSTable(source_graph->vc);
    int *path = malloc(sizeof(int) * (source_graph->arestas + 1));
    int current_path_length = 0;
    int current_v = start_v;

    while (g_copy.arestas > 0)
    {
        Pontes todas_pontes = pontesTarjan(&g_copy, tabela_dfs);
        path[current_path_length++] = current_v;
        int next_v = -1;
        int bridge_v = -1;
        Node *neighbor = g_copy.v[current_v].head;

        while (neighbor != NULL)
        {
            int v = neighbor->id;
            bool is_bridge = isBridgeTarjan(tabela_dfs, current_v, v);
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
        free(todas_pontes.lista);
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

void run_tests() // gerado pelo gemini
{
    int num_tests = 1;
    const char *categories[] = {"eulerians", "semieulerians"};
    // Usamos apenas os tamanhos que você quer testar
    const int sizes[] = {100, 1000, 10000, 100000};
    const int num_categories = sizeof(categories) / sizeof(categories[0]);
    const int num_sizes = sizeof(sizes) / sizeof(sizes[0]);
    const int total_scenarios = num_categories * num_sizes;

    // Array para armazenar o TEMPO TOTAL acumulado para cada cenário
    double *total_times_tarjan = (double *)calloc(total_scenarios, sizeof(double));
    if (!total_times_tarjan)
        exit(EXIT_FAILURE);

    // Array para contar quantas vezes cada cenário rodou com SUCESSO (para a média)
    int *successful_runs = (int *)calloc(total_scenarios, sizeof(int));
    if (!successful_runs)
        exit(EXIT_FAILURE);

    fprintf(stderr, "\n--- INICIANDO BENCHMARKING DE PERFORMANCE (N=%d) ---\n", num_tests);

    // ######################################################
    // 1. LOOP EXTERNO DE REPETIÇÃO (N vezes)
    // ######################################################
    for (int t = 0; t < num_tests; t++)
    {
        int array_index = 0; // Índice para o array de resultados (de 0 a 8)

        for (int k = 0; k < num_categories; k++)
        {
            for (int m = 0; m < num_sizes; m++)
            {
                int V = sizes[m];
                const char *category = categories[k];
                char filename[256];
                snprintf(filename, sizeof(filename), "testing/%s/graph-%d.txt", category, V);

                fprintf(stderr, "\n== SETUP: %s (V=%d) ==\n", category, V);

                Graph g = importGraph(filename);
                if (g.vc == 0)
                {
                    array_index++;
                    continue;
                } // Falha na importação

                // --- MEDIÇÃO ---
                clock_t start_T = clock();
                int path_size_T;
                int *path_tarjan = fleuryTarjan(&g, &path_size_T);
                clock_t end_T = clock();

                double time_T = (double)(end_T - start_T) / CLOCKS_PER_SEC;

                // --- VALIDAÇÃO E ACUMULAÇÃO ---
                bool is_valid_for_fleury = (checkDegrees(&g) != NAO_EULERIANO);
                bool success = (path_tarjan != NULL && path_size_T == g.arestas + 1);

                if (!is_valid_for_fleury) // Se o grafo for NAO_EULERIANO (o Fleury deve falhar)
                {
                    // Não acumulamos tempo (0.0) e não contamos a rodada (é um teste lógico)
                    // Mas se quisermos o tempo zero, podemos fazer a lógica:
                    // total_times_tarjan[array_index] += 0.0;
                }
                else if (success) // Se for EULERIANO/SEMI-EULERIANO e o caminho estiver COMPLETO
                {
                    total_times_tarjan[array_index] += time_T; // Acumula o tempo
                    successful_runs[array_index]++;            // Conta a rodada como SUCESSO
                }
                // Senão, é um erro inesperado (Euleriano falhou), não acumulamos nem contamos.

                // --- LIMPEZA ---
                if (path_tarjan)
                    free(path_tarjan);
                freeGraph(&g);
                array_index++;
            }
        }
    }

    // --- IMPRESSÃO DOS RESULTADOS FINAIS (MÉDIA) ---
    printf("\n--- TEMPOS MÉDIOS DO FLEURY NAIVE (N=%d Rodadas) ---\n", num_tests);
    printf("| V | Tipo | Tempo Medio (s) |\n");
    printf("|---|---|---|\n");

    int array_index = 0;
    for (int m = 0; m < num_sizes; m++) // Tamanho
    {
        for (int k = 0; k < num_categories; k++) // Categoria
        {
            int current_array_index = (k * num_sizes) + m;
            double total_time = total_times_tarjan[current_array_index];
            int runs = successful_runs[array_index];

            // Calcula a média. Se não houver sucesso (runs=0), o tempo é 0.
            double avg_time = (runs > 0) ? (total_time / runs) : 0.0;

            printf("| %d | %s | %.6f |\n", sizes[m], categories[k], avg_time);

            array_index++;
        }
    }

    // Libera os arrays alocados
    free(total_times_tarjan);
    free(successful_runs);
}

int main()
{
    srand(time(NULL));
    run_tests();
    return 0;
}