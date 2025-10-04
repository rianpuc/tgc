#include "graph_generator.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void insertion_sort(Lista *vizinhanca)
{
    if (vizinhanca == NULL || vizinhanca->head == NULL || vizinhanca->head->next == NULL)
    {
        return;
    }
    Node *i = vizinhanca->head->next;
    while (i != NULL)
    {
        Node *chave = i;
        Node *j = i->prev;
        Node *proximo = i->next;
        while (j != NULL && j->id > chave->id)
        {
            j = j->prev;
        }
        if (j != chave->prev)
        {
            chave->prev->next = chave->next;
            if (chave->next != NULL)
            {
                chave->next->prev = chave->prev;
            }
            else
            {
                vizinhanca->tail = chave->prev;
            }
            if (j == NULL)
            {
                chave->next = vizinhanca->head;
                vizinhanca->head->prev = chave;
                chave->prev = NULL;
                vizinhanca->head = chave;
            }
            else
            {
                chave->next = j->next;
                if (j->next != NULL)
                {
                    j->next->prev = chave;
                }
                j->next = chave;
                chave->prev = j;
            }
        }
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

bool arestaExiste(Graph *graph, int origem, int destino)
{
    Lista vizinhanca = graph->v[origem];
    Node *ptr = vizinhanca.head;
    while (ptr != NULL)
    {
        if (ptr->id == destino)
        {
            return true;
        }
        ptr = ptr->next;
    }
    return false;
}

Graph initializeGraph(int vertices) // funcao para inicializar o grafo
{
    Graph grafo;
    grafo.vc = vertices;
    grafo.v = malloc(sizeof(Lista) * vertices);
    grafo.rv = malloc(sizeof(Lista) * vertices);
    grafo.entrada = malloc(sizeof(int) * vertices);
    grafo.saida = malloc(sizeof(int) * vertices);
    grafo.arestas = 0;
    for (int i = 0; i < vertices; i++) // inicializando tudo como null
    {
        grafo.v[i].head = grafo.v[i].tail = NULL;
        grafo.rv[i].head = grafo.rv[i].tail = NULL;
        grafo.entrada[i] = grafo.saida[i] = 0;
    }
    return grafo;
}

void addEdge(Graph *graph, int origem, int destino)
{
    if (origem == destino)
    {
        return;
    }
    if (arestaExiste(graph, origem, destino))
    {
        return;
    }
    Node *dest = malloc(sizeof(Node));
    Node *orig = malloc(sizeof(Node));
    dest->id = destino;
    orig->id = origem;
    dest->next = orig->next = NULL;
    dest->prev = orig->prev = NULL;
    // adicionando o vizinho na lista de adjacencia
    if (graph->v[origem].head == NULL && graph->v[origem].tail == NULL)
    {
        graph->v[origem].head = dest;
        graph->v[origem].tail = dest;
    }
    else if (graph->v[origem].head == graph->v[origem].tail)
    {
        dest->prev = graph->v[origem].head;
        graph->v[origem].head->next = dest;
        graph->v[origem].tail = dest;
    }
    else
    {
        graph->v[origem].tail->next = dest;
        dest->prev = graph->v[origem].tail;
        graph->v[origem].tail = dest;
    }
    // adicionando a origem como predecessor na lista auxiliar para facilitar a busca
    if (graph->rv[destino].head == NULL && graph->rv[destino].tail == NULL)
    {
        graph->rv[destino].head = orig;
        graph->rv[destino].tail = orig;
    }
    else if (graph->rv[destino].head == graph->rv[destino].tail)
    {
        orig->prev = graph->rv[destino].head;
        graph->rv[destino].head->next = orig;
        graph->rv[destino].tail = orig;
    }
    else
    {
        graph->rv[destino].tail->next = orig;
        orig->prev = graph->rv[destino].tail;
        graph->rv[destino].tail = orig;
    }
    graph->saida[origem] += 1;
    graph->entrada[destino] += 1;
}

void populateGraph(Graph *graph, int vertices, bool conexo, Densidade d)
{
    if (conexo && vertices > 1)
    {
        for (int i = 1; i < vertices; i++)
        {

            addEdge(graph, i - 1, i);
            graph->arestas = graph->arestas + 1;
        }
    };
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
    for (int i = 0; i < vertices; i++)
    {
        for (int j = i + 1; j < vertices - 1; j++)
        {
            if (!arestaExiste(graph, i, j) && ((double)rand() / RAND_MAX < prob))
            {
                addEdge(graph, i, j);
                graph->arestas = graph->arestas + 1;
            }
        }
    }
    for (int i = vertices - 1; i > 0; i--)
    {
        for (int j = i - 1; j > 1; j--)
        {
            if (!arestaExiste(graph, i, j) && ((double)rand() / RAND_MAX < prob))
            {
                addEdge(graph, i, j);
                graph->arestas = graph->arestas + 1;
            }
        }
    }
}

Graph_Type checkDegrees(Graph *graph)
{
    int vertices_desbalanceados = 0;
    int saldo_saida_maior = 0;
    int saldo_entrada_maior = 0;
    for (int i = 0; i < graph->vc; i++)
    {
        int diff = graph->saida[i] - graph->entrada[i];
        if (diff != 0)
        {
            if (diff > 1 || diff < -1)
            {
                return NAO_EULERIANO;
            }
            vertices_desbalanceados++;
            if (diff == 1)
            {
                saldo_saida_maior++;
            }
            else
            {
                saldo_entrada_maior++;
            }
        }
    }

    if (vertices_desbalanceados == 0)
    {
        return EULERIANO;
    }
    else if (vertices_desbalanceados == 2 && saldo_saida_maior == 1 && saldo_entrada_maior == 1)
    {
        return SEMI_EULERIANO;
    }
    else
    {
        return NAO_EULERIANO;
    }
}

void acharArestas(Graph *graph, int *u, int *v)
{
    int max_excesso_saida_id = -1;
    int max_excesso_entrada_id = -1;
    int max_excesso_saida = 0;
    int max_excesso_entrada = 0;
    for (int i = 0; i < graph->vc; i++)
    {
        int diff = graph->saida[i] - graph->entrada[i];

        if (diff > 0 && diff > max_excesso_saida)
        {
            max_excesso_saida = diff;
            max_excesso_saida_id = i;
        }
        else if (diff < 0 && abs(diff) > max_excesso_entrada)
        {
            max_excesso_entrada = abs(diff);
            max_excesso_entrada_id = i;
        }
    }
    *u = max_excesso_entrada_id;
    *v = max_excesso_saida_id;
}

void eulerianGraph(Graph *graph)
{
    while (checkDegrees(graph) != EULERIANO)
    {
        int u, v;
        acharArestas(graph, &u, &v);
        addEdge(graph, u, v);
    }
}

void semiEulerianGraph(Graph *graph)
{
    while (checkDegrees(graph) != SEMI_EULERIANO)
    {
        int u, v;
        acharArestas(graph, &u, &v);
        addEdge(graph, u, v);
    }
}

void notEulerianGraph(Graph *graph)
{
    eulerianGraph(graph);
    while (checkDegrees(graph) != NAO_EULERIANO)
    {
        int *vertices_pool = malloc(sizeof(int) * graph->vc);
        for (int i = 0; i < graph->vc; i++)
        {
            vertices_pool[i] = i;
        }

        for (int i = 0; i < graph->vc; i++)
        {
            int j = i + (rand() % (graph->vc - i));
            int temp = vertices_pool[i];
            vertices_pool[i] = vertices_pool[j];
            vertices_pool[j] = temp;
        }

        int u1 = vertices_pool[0];
        int v1 = vertices_pool[1];
        int u2 = vertices_pool[2];
        int v2 = vertices_pool[3];

        addEdge(graph, u1, v1);
        addEdge(graph, u2, v2);
        free(vertices_pool);
    }
}

void printDegrees(Graph *graph)
{
    for (int i = 0; i < graph->vc; i++)
    {
        printf("V: %d [E: %d | S: %d]\n", i + 1, graph->entrada[i], graph->saida[i]);
    }
}

Graph createGraph(int vertices, Densidade d)
{
    Graph graph = initializeGraph(vertices);
    populateGraph(&graph, vertices, false, d);
    ordenarVizinhos(&graph);
    return graph;
}

Graph createGraphConexo(int vertices, Densidade d)
{
    Graph graph = initializeGraph(vertices);
    populateGraph(&graph, vertices, true, d);
    ordenarVizinhos(&graph);
    return graph;
}

Graph createGraphEulerian(int vertices, Densidade d)
{
    Graph graph = createGraphConexo(vertices, d);
    eulerianGraph(&graph);
    return graph;
}

Graph createGraphSemiEulerian(int vertices, Densidade d)
{
    Graph graph = createGraphConexo(vertices, d);
    semiEulerianGraph(&graph);
    return graph;
}

Graph createGraphNotEulerian(int vertices, Densidade d)
{
    Graph graph = createGraphConexo(vertices, d);
    notEulerianGraph(&graph);
    return graph;
}