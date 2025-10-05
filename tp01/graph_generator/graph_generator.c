#include "graph_generator.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#define MAX_VERTICES 100000
#define MAX_NODES (1500000000)
#define MAX_HASH_NODES (750000000)

HashNode *edge_hash_table[HASH_TABLE_SIZE];

typedef struct
{
    int *array;   // O array que armazenará os IDs dos vértices
    int head;     // Índice do primeiro elemento (a ser removido)
    int tail;     // Índice do próximo local livre (onde o novo elemento será inserido)
    int size;     // Número atual de elementos na fila
    int capacity; // Capacidade máxima do array
} Queue;

// --- POOL PARA NÓS DA LISTA DE ADJACÊNCIA ---
Node *node_pool_buffer;
long node_pool_index = 0;

// --- POOL PARA NÓS DA TABELA HASH ---
HashNode *hash_node_pool_buffer;
long hash_node_pool_index = 0;

void freeHashTable()
{
    for (int i = 0; i < HASH_TABLE_SIZE; i++)
    {
        edge_hash_table[i] = NULL;
    }
}

Node *pool_allocate_node(int id)
{
    if (node_pool_index >= MAX_NODES)
    {
        fprintf(stderr, "Erro: Pool de Nodes esgotado!\n");
        exit(EXIT_FAILURE);
    }
    Node *novo = &node_pool_buffer[node_pool_index++];
    novo->id = id;
    novo->next = novo->prev = NULL;
    return novo;
}

HashNode *pool_allocate_hash_node()
{
    if (hash_node_pool_index >= MAX_HASH_NODES)
    {
        fprintf(stderr, "Erro: Pool de HashNodes esgotado!\n");
        exit(EXIT_FAILURE);
    }
    HashNode *novo = &hash_node_pool_buffer[hash_node_pool_index++];
    novo->next = NULL;
    return novo;
}

void initGlobalPools()
{
    if (node_pool_buffer == NULL)
    {
        node_pool_buffer = (Node *)malloc(sizeof(Node) * MAX_NODES);
        hash_node_pool_buffer = (HashNode *)malloc(sizeof(HashNode) * MAX_HASH_NODES);
        if (!node_pool_buffer || !hash_node_pool_buffer)
        {
            perror("FALHA CRÍTICA: Não foi possível alocar o pool de memória");
            exit(EXIT_FAILURE);
        }
    }
}

void resetGlobalState()
{
    node_pool_index = 0;
    hash_node_pool_index = 0;
    freeHashTable();
}

unsigned int hash_edge(int u, int v)
{
    if (u > v)
    {
        int temp = u;
        u = v;
        v = temp;
    }
    unsigned int hash_val = (unsigned int)(u * 92821 + v);
    return hash_val % HASH_TABLE_SIZE;
}

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

void removeOddVertex(Graph *graph, int v)
{
    for (int i = 0; i < graph->odd_count; i++)
    {
        if (graph->odd_vertices_pool[i] == v)
        {
            graph->odd_vertices_pool[i] = graph->odd_vertices_pool[graph->odd_count - 1];
            graph->odd_count--;
            return;
        }
    }
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

bool arestaExiste(int u, int v)
{
    unsigned int index = hash_edge(u, v);
    HashNode *current = edge_hash_table[index];
    while (current != NULL)
    {
        if ((current->u == u && current->v == v) || (current->u == v && current->v == u))
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
    grafo.odd_vertices_pool = malloc(sizeof(int) * vertices);
    grafo.odd_count = 0;
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
    grafo.odd_vertices_pool = malloc(sizeof(int) * vertices);
    grafo.odd_count = 0;
    grafo.vc = vertices;
    grafo.v = malloc(sizeof(Lista) * vertices);
    grafo.grau = malloc(sizeof(int) * vertices);
    grafo.arestas = 0;
    for (int i = 0; i < vertices; i++) // inicializando tudo como null
    {
        grafo.v[i].head = grafo.v[i].tail = NULL;
        grafo.grau[i] = 0;
    }
    if (node_pool_buffer == NULL)
    {
        node_pool_buffer = (Node *)malloc(sizeof(Node) * MAX_NODES);
        hash_node_pool_buffer = (HashNode *)malloc(sizeof(HashNode) * MAX_HASH_NODES);
    }
    node_pool_index = 0;
    hash_node_pool_index = 0;
    if (!node_pool_buffer || !hash_node_pool_buffer)
    {
        perror("FALHA CRÍTICA: Não foi possível alocar o pool de memória");
        exit(EXIT_FAILURE);
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

void removeEdgeLocal(Graph *graph, int u, int v)
{
    removeNodeFromList(&graph->v[u], v);
    removeNodeFromList(&graph->v[v], u);
    if (graph->grau[u] % 2 != 0)
    {
        removeOddVertex(graph, u);
    }
    else
    {
        graph->odd_vertices_pool[graph->odd_count++] = u;
    }
    if (graph->grau[v] % 2 != 0)
    {
        removeOddVertex(graph, v);
    }
    else
    {
        graph->odd_vertices_pool[graph->odd_count++] = v;
    }
    graph->grau[u] -= 1;
    graph->grau[v] -= 1;
    graph->arestas -= 1;
}

void removeEdge(Graph *graph, int u, int v)
{
    unsigned int index = hash_edge(u, v);
    HashNode *current = edge_hash_table[index];
    HashNode *prev = NULL;

    while (current != NULL)
    {
        if ((current->u == u && current->v == v) || (current->u == v && current->v == u))
        {
            if (prev == NULL)
            {
                edge_hash_table[index] = current->next;
            }
            else
            {
                prev->next = current->next;
            }
            free(current);
            break;
        }
        prev = current;
        current = current->next;
    }

    removeNodeFromList(&graph->v[u], v);
    removeNodeFromList(&graph->v[v], u);

    if (graph->grau[u] % 2 != 0)
    {
        removeOddVertex(graph, u);
    }
    else
    {
        graph->odd_vertices_pool[graph->odd_count++] = u;
    }
    graph->grau[u] -= 1;

    if (graph->grau[v] % 2 != 0)
    {
        removeOddVertex(graph, v);
    }
    else
    {
        graph->odd_vertices_pool[graph->odd_count++] = v;
    }
    graph->grau[v] -= 1;
    graph->arestas -= 1;
}

void addEdgeToListImport(Lista *list, int id)
{
    Node *novo = (Node *)malloc(sizeof(Node));
    if (!novo)
    {
        perror("Falha de alocacao para Node");
        exit(EXIT_FAILURE);
    }

    novo->id = id;
    novo->next = NULL;
    novo->prev = NULL;

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

void addEdgeToList(Lista *list, int id)
{
    // Node *novo = malloc(sizeof(Node));
    Node *novo = pool_allocate_node(id);
    novo->id = id;
    novo->next = novo->prev = NULL;
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

void addEdgeImport(Graph *graph, int u, int v)
{
    if (u == v || arestaExiste(u, v))
    {
        return;
    }

    bool u_era_impar = (graph->grau[u] % 2 != 0);
    bool v_era_impar = (graph->grau[v] % 2 != 0);

    addEdgeToListImport(&graph->v[u], v);
    addEdgeToListImport(&graph->v[v], u);

    graph->arestas += 1;
    graph->grau[u] += 1;
    graph->grau[v] += 1;

    if (u_era_impar)
    {
        removeOddVertex(graph, u);
    }
    else
    {
        graph->odd_vertices_pool[graph->odd_count++] = u;
    }
    if (v_era_impar)
    {
        removeOddVertex(graph, v);
    }
    else
    {
        graph->odd_vertices_pool[graph->odd_count++] = v;
    }
}

void addEdge(Graph *graph, int u, int v)
{
    if (u == v || arestaExiste(u, v))
    {
        return;
    }

    bool u_era_impar = (graph->grau[u] % 2 != 0);
    bool v_era_impar = (graph->grau[v] % 2 != 0);

    addEdgeToList(&graph->v[u], v);
    addEdgeToList(&graph->v[v], u);

    unsigned int index = hash_edge(u, v);
    HashNode *newNode = pool_allocate_hash_node();
    if (u < v)
    {
        newNode->u = u;
        newNode->v = v;
    }
    else
    {
        newNode->u = v;
        newNode->v = u;
    }
    newNode->next = edge_hash_table[index];
    edge_hash_table[index] = newNode;

    graph->arestas += 1;
    graph->grau[u] += 1;
    graph->grau[v] += 1;

    if (u_era_impar)
    {
        removeOddVertex(graph, u);
    }
    else
    {
        graph->odd_vertices_pool[graph->odd_count++] = u;
    }
    if (v_era_impar)
    {
        removeOddVertex(graph, v);
    }
    else
    {
        graph->odd_vertices_pool[graph->odd_count++] = v;
    }
}

void populateGraph(Graph *graph, int vertices, Densidade d)
{
    double prob = 0;
    switch (d)
    {
    case ESPARSO:
        prob = 0.00006;
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
                    addEdge(graph, i, j);
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
            addEdge(graph, i, target);
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
        addEdge(graph, a, b);
    }
    free(visited);
    free(reps_list);
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

bool acharArestas(Graph *graph, int *u, int *v)
{
    int count = graph->odd_count;
    if (count < 2)
        return false;
    for (int i = 0; i < count; i++)
    {
        int u_impar = graph->odd_vertices_pool[i];
        for (int j = i + 1; j < count; j++)
        {
            int v_impar = graph->odd_vertices_pool[j];
            if (!arestaExiste(u_impar, v_impar))
            {
                *u = u_impar;
                *v = v_impar;
                return true;
            }
        }
    }
    if (count == 2)
    {
        *u = graph->odd_vertices_pool[0];
        int v_impar = graph->odd_vertices_pool[1];
        for (int w = 0; w < graph->vc; w++)
        {
            if (graph->grau[w] % 2 == 0 && w != *u && w != v_impar)
            {
                if (!arestaExiste(*u, w))
                {
                    *v = w;
                    return true;
                }
            }
        }
    }
    return false;
}

bool acharArestasSimples(Graph *graph, int *u, int *v)
{
    for (int i = 0; i < graph->vc; i++)
    {
        for (int j = i + 1; j < graph->vc; j++)
        {
            if (!arestaExiste(i, j))
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
        if (acharArestas(graph, &u, &v))
        {
            addEdge(graph, u, v);
        }
    }
}

void semiEulerianGraph(Graph *graph)
{
    while (checkDegrees(graph) != SEMI_EULERIANO)
    {
        int u, v;
        if (acharArestas(graph, &u, &v))
        {
            addEdge(graph, u, v);
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
            addEdge(graph, u1, v1);
        }
        else
        {
            fprintf(stderr, "Erro de Densidade: Nao foi possivel encontrar aresta para quebra.\n");
            break;
        }
        if (acharArestasSimples(graph, &u2, &v2))
        {
            addEdge(graph, u2, v2);
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

Graph copyGraph(Graph *source)
{
    Graph g;
    g.vc = source->vc;
    g.arestas = source->arestas; // Copia a contagem final de arestas
    g.odd_vertices_pool = malloc(sizeof(int) * source->vc);
    g.odd_count = source->odd_count;
    g.grau = (int *)malloc(sizeof(int) * source->vc);
    g.v = (Lista *)malloc(sizeof(Lista) * source->vc);

    for (int i = 0; i < source->vc; i++)
    {
        g.v[i].head = g.v[i].tail = NULL;
        g.grau[i] = source->grau[i]; // Copia os graus finais
    }

    for (int i = 0; i < g.vc; i++)
    {
        Node *ptr = source->v[i].head;
        while (ptr != NULL)
        {
            Node *novo_no = (Node *)malloc(sizeof(Node));
            if (!novo_no)
                exit(EXIT_FAILURE);

            novo_no->id = ptr->id;
            novo_no->next = novo_no->prev = NULL;
            if (g.v[i].head == NULL)
            {
                g.v[i].head = g.v[i].tail = novo_no;
            }
            else
            {
                g.v[i].tail->next = novo_no;
                novo_no->prev = g.v[i].tail;
                g.v[i].tail = novo_no;
            }

            ptr = ptr->next;
        }
    }
    return g;
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

Graph createHaltereGraph()
{

    int vertices = 2 * 5;
    Graph graph = initializeGraph(vertices);

    for (int i = 0; i < 5; i++)
    {
        int u = i;
        int v = (i + 1) % 5;
        addEdge(&graph, u, v);
    }

    int offset = 5;
    for (int i = 0; i < 5; i++)
    {
        int u = i + offset;
        int v = ((i + 1) % 5) + offset;
        addEdge(&graph, u, v);
    }

    int ponte_u = 5 - 1;
    int ponte_v = 5;

    addEdge(&graph, ponte_u, ponte_v);
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
                fprintf(fp, "%d %d\n", u, v);
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
    freeHashTable();
    node_pool_index = 0;
    hash_node_pool_index = 0;
    int vertices, arestas_total;
    if (fscanf(fp, "%d %d", &vertices, &arestas_total) != 2)
    {
        fprintf(stderr, "Erro de formato no cabecalho do arquivo.\n");
        fclose(fp);
        Graph empty_graph = {.vc = 0};
        return empty_graph;
    }
    Graph graph = simpleInitialization(vertices);
    int u, v;
    int edges_read = 0;
    while (fscanf(fp, "%d %d", &u, &v) == 2)
    {
        addEdgeImport(&graph, u, v);
        edges_read++;
    }
    fclose(fp);
    ordenarVizinhos(&graph);
    return graph;
}