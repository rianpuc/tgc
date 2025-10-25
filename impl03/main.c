#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#define MAX_PESO 25

typedef struct
{
    int *distancia;
    int *pred;
} Dijkstra;

typedef struct HeapNode
{
    int vertice;
    int peso;
} HeapNode;

typedef struct Heap
{
    int capacidade;
    int atuais;
    int *pos;
    HeapNode **vetor;
} Heap;

typedef struct Node
{
    int vizinho;
    int weight;
    struct Node *prox;
} Node;

typedef struct Graph
{
    int vc;
    Node **v;
} Graph;

int get_parent(int pos)
{
    return (pos - 1) / 2;
}

int get_left_child(int pos)
{
    return (2 * pos) + 1;
}

int get_right_child(int pos)
{
    return (2 * pos) + 2;
}

void swap_heapnode(HeapNode **a, HeapNode **b)
{
    HeapNode *temp = *a;
    *a = *b;
    *b = temp;
}

void swap_node(Node **a, Node **b)
{
    Node *temp = *a;
    *a = *b;
    *b = temp;
}

HeapNode *create_heap_node(int vertice, int peso)
{
    HeapNode *tmp = malloc(sizeof(HeapNode));
    tmp->vertice = vertice;
    tmp->peso = peso;
    return tmp;
}

Heap initialize_heap(int capacidade)
{
    Heap h;
    h.capacidade = capacidade;
    h.atuais = 0;
    h.vetor = malloc(sizeof(HeapNode) * capacidade);
    h.pos = malloc(sizeof(int) * capacidade);
    for (int i = 0; i < capacidade; i++)
    {
        h.vetor[i] = NULL;
        h.pos[i] = -1;
    }
    return h;
}

void insert_heap(Heap *heap, int vertice, int peso)
{
    if (heap->capacidade == heap->atuais)
    {
        perror("Heap cheio.\n");
        return;
    }
    printf("Inserindo V: %d, P: %d\n", vertice, peso);
    HeapNode *tmp = create_heap_node(vertice, peso);
    int i = heap->atuais;
    heap->pos[vertice] = i;
    heap->vetor[i] = tmp;
    heap->atuais++;
    while (i != 0 && heap->vetor[i]->peso < heap->vetor[get_parent(i)]->peso)
    {
        int id_pai = get_parent(i);
        HeapNode *filho = heap->vetor[i];
        HeapNode *pai = heap->vetor[id_pai];
        heap->pos[filho->vertice] = id_pai;
        heap->pos[pai->vertice] = i;
        swap_heapnode(&heap->vetor[id_pai], &heap->vetor[i]);
        i = id_pai;
    }
}

void min_heapify_down(Heap *heap, int id)
{
    int menor = id;
    int esq = get_left_child(id);
    int dir = get_right_child(id);
    if (esq < heap->atuais && heap->vetor[menor]->peso > heap->vetor[esq]->peso)
    {
        menor = esq;
    }
    if (dir < heap->atuais && heap->vetor[menor]->peso > heap->vetor[dir]->peso)
    {
        menor = dir;
    }
    if (menor != id)
    {
        HeapNode *no_menor = heap->vetor[menor];
        HeapNode *no_maior = heap->vetor[id];
        heap->pos[no_menor->vertice] = id;
        heap->pos[no_maior->vertice] = menor;
        swap_heapnode(&heap->vetor[id], &heap->vetor[menor]);
        min_heapify_down(heap, menor);
    }
}

HeapNode *extract_min(Heap *heap)
{
    if (heap->atuais == 0)
    {
        perror("Heap vazio.\n");
        return NULL;
    }
    HeapNode *raiz = heap->vetor[0];
    HeapNode *ultimo = heap->vetor[heap->atuais - 1];
    heap->vetor[0] = ultimo;
    heap->pos[raiz->vertice] = -1;
    heap->pos[ultimo->vertice] = 0;
    heap->atuais--;
    min_heapify_down(heap, 0);
    return raiz;
}

void decrease_key(Heap *heap, int vertice, int distancia)
{
    int i = heap->pos[vertice];
    heap->vetor[i]->peso = distancia;
    while (i != 0 && heap->vetor[i]->peso < heap->vetor[get_parent(i)]->peso)
    {
        int parent_id = get_parent(i);
        HeapNode *filho = heap->vetor[i];
        HeapNode *pai = heap->vetor[parent_id];
        heap->pos[filho->vertice] = parent_id;
        heap->pos[pai->vertice] = i;
        swap_heapnode(&heap->vetor[parent_id], &heap->vetor[i]);
        i = parent_id;
    }
}

Graph initialize_graph(int vertices)
{
    Graph grafo;
    grafo.vc = vertices;
    grafo.v = malloc(sizeof(Node *) * vertices);
    for (int i = 0; i < vertices; i++)
    {
        grafo.v[i] = NULL;
    }
    return grafo;
}

Node *create_node(int vizinho, int peso)
{
    Node *tmp = malloc(sizeof(Node));
    tmp->vizinho = vizinho;
    tmp->weight = peso;
    tmp->prox = NULL;
    return tmp;
}

void add_edge(Graph *grafo, int origem, int destino, int peso)
{
    Node *tmp = create_node(destino, peso);
    tmp->prox = grafo->v[origem];
    grafo->v[origem] = tmp;
}

void add_undirected_edge(Graph *grafo, int v1, int v2, int peso)
{
    add_edge(grafo, v1, v2, peso);
    add_edge(grafo, v2, v1, peso);
}

void print_graph(Graph *grafo)
{
    printf("Visualizacao do Grafo:\n");
    for (int i = 0; i < grafo->vc; i++)
    {
        printf("Vertice %d do Grafo\n", i);
        Node *ptr = grafo->v[i];
        printf("Vizinhos: ");
        while (ptr != NULL)
        {
            printf(" (%d, P: %d) ->", ptr->vizinho, ptr->weight);
            ptr = ptr->prox;
        }
        printf("\n-----Fim da lista de vizinhos para Vertice: %d-----\n", i);
    }
}

bool is_in_heap(Heap *h, int vertice)
{
    bool res = false;
    if (h->pos[vertice] < h->atuais)
    {
        res = true;
    }
    return res;
}

bool todosVisitados(bool *visited, int max)
{
    for (int i = 0; i < max; i++)
    {
        if (visited[i] == false)
            return false;
    }
    return true;
}

int menorAtual(int *distancias, bool *visitados, int v)
{
    int menor_distancia = INT_MAX;
    int menor_indice = -1;
    for (int i = 0; i < v; i++)
    {
        if (visitados[i] == false && menor_distancia >= distancias[i])
        {
            menor_distancia = distancias[i];
            menor_indice = i;
        }
    }
    return menor_indice;
}

void build_min_heap(Heap *heap)
{
    int n = heap->atuais;
    for (int i = (n / 2) - 1; i >= 0; i--)
    {
        min_heapify_down(heap, i);
    }
}

int *dijkstra_sequencial(Graph *grafo, int origem)
{
    int *res = malloc(sizeof(int) * (grafo->vc));
    bool *visited = malloc(sizeof(bool) * (grafo->vc));
    for (int i = 0; i < grafo->vc; i++)
    {
        res[i] = INT_MAX;
        visited[i] = false;
    }
    res[origem] = 0;
    while (!todosVisitados(visited, grafo->vc))
    {
        int atual = menorAtual(res, visited, grafo->vc);
        if (res[atual] == -1 || atual == -1)
            break;
        visited[atual] = true;
        Node *ptr = grafo->v[atual];
        while (ptr != NULL)
        {
            int distancia = res[atual] + ptr->weight;
            if (visited[ptr->vizinho] != true && res[ptr->vizinho] > distancia)
            {
                res[ptr->vizinho] = distancia;
            }
            ptr = ptr->prox;
        }
    }
    free(visited);
    return res;
}

int *dijkstra_otimizado(Graph *grafo, int origem)
{
    int V = grafo->vc;
    int *res = malloc(sizeof(int) * (V));
    Heap h = initialize_heap(V);
    res[origem] = 0;
    for (int i = 0; i < V; i++)
    {
        res[i] = (i == origem ? res[i] : INT_MAX);
        h.vetor[i] = create_heap_node(i, res[i]);
        h.pos[i] = i;
    }
    res[origem] = 0;
    h.vetor[origem]->peso = 0;
    h.atuais = V;
    build_min_heap(&h);
    while (h.atuais > 0)
    {
        HeapNode *min = extract_min(&h);
        int atual = min->vertice;
        if (res[atual] == INT_MAX)
            break;
        Node *ptr = grafo->v[atual];
        while (ptr != NULL)
        {
            int distancia = res[atual] + ptr->weight;
            if (is_in_heap(&h, ptr->vizinho) && res[ptr->vizinho] > distancia)
            {
                res[ptr->vizinho] = distancia;
                decrease_key(&h, ptr->vizinho, distancia);
            }
            ptr = ptr->prox;
        }
        free(min);
    }
    return res;
}

void initialize_edges(Graph *grafo, int max_arestas)
{
    int V = grafo->vc;
    for (int i = 0; i < max_arestas; i++)
    {
        int destino, origem;
        do
        {
            origem = rand() % V;
            destino = rand() % V;
        } while (destino == origem);
        int peso = (rand() % MAX_PESO) + 1;
        add_edge(grafo, origem, destino, peso);
    }
}

void print_results(int *pos, int n)
{
    for (int i; i < n; i++)
    {
        printf("%d ", pos[i]);
    }
    printf("\n");
}

int main(int argc, char **argv)
{
    srand(time(NULL));
    int vertices = atoi(argv[argc - 3]);
    int max_arestas = atoi(argv[argc - 2]);
    int testes = atoi(argv[argc - 1]);
    printf("-- Dijkstra sequencial\n-Vertices: %d\n-Maximo de Arestas: %d\n-Numero de Testes: %d\nIniciando...\n", vertices, max_arestas, testes);
    Graph grafo = initialize_graph(vertices);
    initialize_edges(&grafo, max_arestas);
    double soma = 0;
    double *tempos = malloc(sizeof(double) * testes);
    int i = 0;
    while (testes > i++)
    {
        clock_t start = clock();
        int *distancias = dijkstra_sequencial(&grafo, 0);
        clock_t end = clock();
        double tempo = (double)(end - start) / CLOCKS_PER_SEC;
        tempos[i - 1] = tempo;
        soma += tempo;
        // if(i == testes) print_results(distancias, vertices);
        free(distancias);
    }
    double media = soma / testes;
    double soma_diferencias_quadradas = 0;
    for (int j = 0; j < testes; j++)
    {
        soma_diferencias_quadradas += pow(tempos[j] - media, 2);
    }
    double variancia = soma_diferencias_quadradas / testes;
    double desvio_padrao = sqrt(variancia);
    printf("----- Benchmark:\n");
    printf("Tempo médio: %.6fs | Desvio Padrão: %.6fs\n\n", media, desvio_padrao);
    printf("-- Dijkstra otimizado\n-Vertices: %d\n-Maximo de Arestas: %d\n-Numero de Testes: %d\nIniciando...\n", vertices, max_arestas, testes);
    soma = 0;
    i = 0;
    while (testes > i++)
    {
        clock_t start = clock();
        int *distancias = dijkstra_otimizado(&grafo, 0);
        clock_t end = clock();
        double tempo = (double)(end - start) / CLOCKS_PER_SEC;
        tempos[i - 1] = tempo;
        soma += tempo;
        // if(i == testes) print_results(distancias, vertices);
        free(distancias);
    }
    media = soma / testes;
    soma_diferencias_quadradas = 0;
    for (int j = 0; j < testes; j++)
    {
        soma_diferencias_quadradas += pow(tempos[j] - media, 2);
    }
    variancia = soma_diferencias_quadradas / testes;
    desvio_padrao = sqrt(variancia);
    printf("\n----- Benchmark:\n");
    printf("Tempo médio: %.6fs | Desvio Padrão: %.6fs\n\n", media, desvio_padrao);
    free(tempos);
    return 0;
}