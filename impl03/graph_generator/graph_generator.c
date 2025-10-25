#include "graph_generator.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#define MAX_PESO 100

typedef struct
{
    int *array;   // O array que armazenará os IDs dos vértices
    int head;     // Índice do primeiro elemento (a ser removido)
    int tail;     // Índice do próximo local livre (onde o novo elemento será inserido)
    int size;     // Número atual de elementos na fila
    int capacity; // Capacidade máxima do array
} Queue;

void init_queue(Queue *q, int capacity)
{
    q->array = (int *)malloc(sizeof(int) * capacity);
    if (q->array == NULL)
    {
        perror("Falha na alocacao da fila");
        exit(EXIT_FAILURE);
    }
    q->capacity = capacity;
    q->head = 0;
    q->tail = 0;
    q->size = 0;
}

bool q_isEmpty(Queue *q)
{
    return q->size == 0;
}

void q_push(Queue *q, int item)
{
    if (q->size == q->capacity)
    {
        fprintf(stderr, "Erro: Fila cheia!\n");
        return;
    }

    q->array[q->tail] = item;
    q->tail = (q->tail + 1) % q->capacity;
    q->size++;
}

int q_pop(Queue *q)
{
    if (q_isEmpty(q))
    {
        fprintf(stderr, "Erro: Tentativa de remover de fila vazia.\n");
        return -1;
    }

    int item = q->array[q->head];
    q->head = (q->head + 1) % q->capacity;
    q->size--;

    return item;
}

void free_queue(Queue *q)
{
    free(q->array);
    q->array = NULL;
    q->head = q->tail = q->size = q->capacity = 0;
}

void insertion_sort(Lista *vizinhanca) // Gerado com Gemini
{
    // Checagem de base:
    if (vizinhanca == NULL || vizinhanca->head == NULL || vizinhanca->head->next == NULL)
    {
        return;
    }

    // A. Começa do primeiro nó (vizinhanca->head), permitindo que ele seja movido.
    Node *i = vizinhanca->head;

    while (i != NULL)
    {
        Node *chave = i;
        Node *j = i->prev;
        Node *proximo = i->next; // Guarda o próximo para continuar o loop

        // 1. Busca o ponto de inserção: move 'j' para trás enquanto os IDs de 'j' forem maiores que 'chave'.
        while (j != NULL && j->id > chave->id)
        {
            j = j->prev;
        }

        // Se 'j' é diferente de 'chave->prev', a chave precisa ser movida.
        if (j != chave->prev)
        {
            // --- A. FASE DE ISOLAMENTO (Remover 'chave' da posição atual) ---

            // 1. Atualizar o ponteiro 'next' do predecessor da 'chave'
            if (chave->prev != NULL)
            {
                // Se não for o head, o anterior aponta para o próximo
                chave->prev->next = chave->next;
            }
            else
            {
                // Se a CHAVE FOR O HEAD, o novo head é o nó seguinte
                vizinhanca->head = chave->next;
            }

            // 2. Atualizar o ponteiro 'prev' do sucessor da 'chave'
            if (chave->next != NULL)
            {
                chave->next->prev = chave->prev;
            }
            else
            {
                // Se a CHAVE FOR O TAIL, o novo tail é o nó anterior
                vizinhanca->tail = chave->prev;
            }

            // --- B. FASE DE REINSERÇÃO (Após 'j') ---

            if (j == NULL)
            {
                // Caso 1: Inserção no início (A chave se torna o novo HEAD)
                chave->next = vizinhanca->head;
                if (vizinhanca->head != NULL)
                {
                    vizinhanca->head->prev = chave;
                }
                chave->prev = NULL;
                vizinhanca->head = chave;

                // Se a lista agora tem um único item, ele é o head E o tail.
                if (chave->next == NULL)
                {
                    vizinhanca->tail = chave;
                }
            }
            else
            {
                // Caso 2: Inserção no meio ou no final (após 'j')
                chave->next = j->next;

                if (j->next != NULL)
                {
                    j->next->prev = chave;
                }
                else
                {
                    // Se j->next era NULL, chave é o novo TAIL
                    vizinhanca->tail = chave;
                }

                j->next = chave;
                chave->prev = j;
            }
        }

        // 3. AVANÇAR
        i = proximo;
    }
}

void ordenarVizinhos(Graph *graph)
{
    int n = graph->vc;
    for (int i = 0; i < n; i++)
    {
        insertion_sort(&graph->v[i]);
    }
}

bool arestaExiste(Graph *graph, int u, int v)
{
    Node *current = graph->v[u].head;
    while (current != NULL)
    {
        if (current->id == v)
        {
            return true;
        }
        current = current->next;
    }
    return false;
}

Graph simpleInitialization(int vertices)
{
    Graph grafo;
    grafo.vc = vertices;
    grafo.v = malloc(sizeof(Lista) * vertices);
    grafo.grau = malloc(sizeof(int) * vertices);
    grafo.arestas = 0;
    for (int i = 0; i < vertices; i++) // inicializando tudo como null
    {
        grafo.v[i].head = grafo.v[i].tail = NULL;
        grafo.grau[i] = 0;
    }
    return grafo;
}

Graph initializeGraph(int vertices) // funcao para inicializar o grafo
{
    Graph grafo;
    grafo.vc = vertices;
    grafo.v = malloc(sizeof(Lista) * vertices);
    grafo.grau = malloc(sizeof(int) * vertices);
    grafo.arestas = 0;
    for (int i = 0; i < vertices; i++) // inicializando tudo como null
    {
        grafo.v[i].head = grafo.v[i].tail = NULL;
        grafo.grau[i] = 0;
    }
    return grafo;
}

void removeNodeFromList(Lista *list, int id)
{
    Node *current = list->head;
    Node *prev = NULL;

    while (current != NULL && current->id != id)
    {
        prev = current;
        current = current->next;
    }
    if (current == NULL)
        return;

    if (prev == NULL)
    {
        list->head = current->next;
    }
    else
    {
        prev->next = current->next;
    }

    if (current->next == NULL)
    {
        list->tail = prev;
    }
}

void addEdgeToList(Lista *list, int id, int weight)
{
    Node *novo = malloc(sizeof(Node));
    novo->id = id;
    novo->next = novo->prev = NULL;
    novo->weight = weight;
    if (list->head == NULL)
    {
        list->head = list->tail = novo;
    }
    else
    {
        list->tail->next = novo;
        novo->prev = list->tail;
        list->tail = novo;
    }
}

void addEdge(Graph *graph, int u, int v, int weight)
{
    if (u == v || arestaExiste(graph, u, v))
    {
        return;
    }
    addEdgeToList(&graph->v[u], v, weight);
    graph->arestas += 1;
    graph->grau[u] += 1;
}

void addEdgeImport(Graph *graph, int u, int v, int weight)
{
    addEdgeToList(&graph->v[u], v, weight);
    graph->arestas += 1;
    graph->grau[u] += 1;
}

void populateGraph(Graph *graph, int vertices, Densidade d)
{
    double prob = 0;
    switch (d)
    {
    case ESPARSO:
        prob = 0.15;
        break;
    case MEDIO:
        prob = 0.45;
        break;
    case DENSO:
        prob = 0.75;
        break;
    default:
        prob = 0.15;
        break;
    }

    int prob_limit = (int)(prob * RAND_MAX);
    for (int i = 0; i < vertices; i++)
    {
        for (int j = i + 1; j < vertices; j++)
        {
            if ((rand() < prob_limit))
            {
                {
                    int peso = (rand() % MAX_PESO) + 1;
                    addEdge(graph, i, j, peso);
                }
            }
        }
    }

    bool *visited = malloc(sizeof(bool) * vertices);
    int *reps_list = (int *)malloc(sizeof(int) * vertices);
    if (!visited || !reps_list)
    {
        perror("Falha na alocacao auxiliar");
        exit(EXIT_FAILURE);
    }
    int reps_count = 0;
    for (int i = 0; i < vertices; i++)
    {
        visited[i] = false;
        if (graph->grau[i] == 0)
        {
            int target = rand() % vertices;
            if (target == i)
                target = (i + 1) % vertices;
            int peso = (rand() % MAX_PESO) + 1;
            addEdge(graph, i, target, peso);
        }
    }

    for (int i = 0; i < vertices; i++)
    {
        if (!visited[i])
        {
            reps_list[reps_count++] = i;

            Queue q;
            init_queue(&q, vertices);
            q_push(&q, i);
            visited[i] = true;

            while (!q_isEmpty(&q))
            {
                int u = q_pop(&q);
                Node *ptr = graph->v[u].head;

                while (ptr != NULL)
                {
                    int v = ptr->id;
                    if (!visited[v])
                    {
                        visited[v] = true;
                        q_push(&q, v);
                    }
                    ptr = ptr->next;
                }
            }
            free_queue(&q);
        }
    }
    for (int i = 0; i + 1 < reps_count; i++)
    {
        int a = reps_list[i];
        int b = reps_list[i + 1];
        int peso = (rand() % MAX_PESO) + 1;
        addEdge(graph, a, b, peso);
    }
    free(visited);
    free(reps_list);
}

void completeGraph(Graph *graph, int vertices)
{
    for (int i = 0; i < vertices; i++)
    {
        for (int j = 0; j < vertices; j++)
        {
            if (i != j)
            {
                int peso = (rand() % MAX_PESO) + 1;
                addEdge(graph, i, j, peso);
            }
        }
    }
}

Graph_Type checkDegrees(Graph *graph)
{
    int odd_degree_vertices = 0;
    for (int i = 0; i < graph->vc; i++)
    {
        if (graph->grau[i] % 2 != 0)
        {
            odd_degree_vertices++;
        }
    }
    if (odd_degree_vertices == 0)
    {
        return EULERIANO;
    }
    else if (odd_degree_vertices == 2)
    {
        return SEMI_EULERIANO;
    }
    return NAO_EULERIANO;
}

bool acharArestasSimples(Graph *graph, int *u, int *v)
{
    for (int i = 0; i < graph->vc; i++)
    {
        for (int j = i + 1; j < graph->vc; j++)
        {
            if (!arestaExiste(graph, i, j))
            {
                *u = i;
                *v = j;
                return true;
            }
        }
    }
    return false;
}

void eulerianGraph(Graph *graph)
{
    while (checkDegrees(graph) != EULERIANO)
    {
        int u, v;
        if (acharArestasSimples(graph, &u, &v))
        {
            int peso = (rand() % MAX_PESO) + 1;
            addEdge(graph, u, v, peso);
        }
    }
}

void semiEulerianGraph(Graph *graph)
{
    while (checkDegrees(graph) != SEMI_EULERIANO)
    {
        int u, v;
        if (acharArestasSimples(graph, &u, &v))
        {
            int peso = (rand() % MAX_PESO) + 1;
            addEdge(graph, u, v, peso);
        }
    }
    // for (int i = 0; i < odd_count; i++)
    // {
    //     printf("%d\n", odd_vertices_pool[i]);
    //     printf("degree: %d\n", graph->grau[odd_vertices_pool[i]]);
    // }
}

void notEulerianGraph(Graph *graph)
{
    eulerianGraph(graph);
    int *vertices_pool = malloc(sizeof(int) * graph->vc);
    if (!vertices_pool)
    {
        perror("Falha na alocacao do pool de vertices");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < graph->vc; i++)
    {
        vertices_pool[i] = i;
    }

    while (checkDegrees(graph) != NAO_EULERIANO)
    {
        int u1, v1;
        int u2, v2;

        if (acharArestasSimples(graph, &u1, &v1))
        {
            int peso = (rand() % MAX_PESO) + 1;
            addEdge(graph, u1, v1, peso);
        }
        else
        {
            fprintf(stderr, "Erro de Densidade: Nao foi possivel encontrar aresta para quebra.\n");
            break;
        }
        if (acharArestasSimples(graph, &u2, &v2))
        {
            int peso = (rand() % MAX_PESO) + 1;
            addEdge(graph, u2, v2, peso);
        }
        else
        {
            break;
        }
    }
    free(vertices_pool);
}

void printDegrees(Graph *graph)
{
    for (int i = 0; i < graph->vc; i++)
    {
        printf("V: %d [d: %d]\n", i + 1, graph->grau[i]);
    }
}

Graph createGraph(int vertices, Densidade d)
{
    Graph graph = initializeGraph(vertices);
    populateGraph(&graph, vertices, d);
    // ordenarVizinhos(&graph);
    return graph;
}

Graph createGraphConexo(int vertices, Densidade d)
{
    Graph graph = initializeGraph(vertices);
    populateGraph(&graph, vertices, d);
    // ordenarVizinhos(&graph);
    return graph;
}

Graph createGraphCompleto(int vertices)
{
    Graph graph = initializeGraph(vertices);
    completeGraph(&graph, vertices);
    return graph;
}

Graph createGraphEulerian(int vertices, Densidade d)
{
    Graph graph = createGraphConexo(vertices, d);
    eulerianGraph(&graph);
    ordenarVizinhos(&graph);
    return graph;
}

Graph createGraphSemiEulerian(int vertices, Densidade d)
{
    Graph graph = createGraphConexo(vertices, d);
    semiEulerianGraph(&graph);
    ordenarVizinhos(&graph);
    return graph;
}

Graph createGraphNotEulerian(int vertices, Densidade d)
{
    Graph graph = createGraphConexo(vertices, d);
    notEulerianGraph(&graph);
    ordenarVizinhos(&graph);
    return graph;
}

void exportGraph(Graph *graph, const char *filename)
{
    FILE *fp = fopen(filename, "w");
    fprintf(fp, "%d %d\n", graph->vc, graph->arestas);
    for (int u = 0; u < graph->vc; u++)
    {
        Node *ptr = graph->v[u].head;
        while (ptr != NULL)
        {
            int v = ptr->id;
            if (u < v)
            {
                fprintf(fp, "%d %d %d\n", u, v, ptr->weight);
            }
            ptr = ptr->next;
        }
    }
    fclose(fp);
    fprintf(stderr, "Grafo exportado com sucesso para %s.\n", filename);
}

Graph importGraph(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    int vertices, arestas_total;
    if (fscanf(fp, "%d %d", &vertices, &arestas_total) != 2)
    {
        fprintf(stderr, "Erro de formato no cabecalho do arquivo.\n");
        fclose(fp);
        Graph empty_graph = {.vc = 0};
        return empty_graph;
    }
    Graph graph = simpleInitialization(vertices);
    int u, v, w;
    int edges_read = 0;
    while (fscanf(fp, "%d %d %d", &u, &v, &w) == 3)
    {
        addEdgeImport(&graph, u, v, w);
        edges_read++;
    }
    fclose(fp);
    ordenarVizinhos(&graph);
    return graph;
}