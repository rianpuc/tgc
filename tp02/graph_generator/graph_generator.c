#include "graph_generator.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <limits.h>
#include <time.h>

#define INF (INT_MAX / 2)

void calcularTodasDistancias(Graph *graph)
{
    int V = graph->vc;
    graph->distancias = malloc(V * sizeof(int *));
    for (int i = 0; i < V; i++)
    {
        graph->distancias[i] = malloc(V * sizeof(int));
        for (int j = 0; j < V; j++)
        {
            if (i == j)
                graph->distancias[i][j] = 0;
            else
                graph->distancias[i][j] = INF;
        }
    }
    for (int i = 0; i < V; i++)
    {
        Node *vizinho = graph->v[i].head;
        while (vizinho != NULL)
        {
            graph->distancias[i][vizinho->id] = vizinho->weight;
            graph->distancias[vizinho->id][i] = vizinho->weight;
            vizinho = vizinho->next;
        }
    }
    for (int k = 0; k < V; k++)
    {
        for (int i = 0; i < V; i++)
        {
            for (int j = 0; j < V; j++)
            {
                if (graph->distancias[i][k] + graph->distancias[k][j] < graph->distancias[i][j])
                {
                    graph->distancias[i][j] = graph->distancias[i][k] + graph->distancias[k][j];
                }
            }
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

Graph simpleInitialization(int vertices)
{
    Graph grafo;
    grafo.vc = vertices;
    grafo.v = NULL;
    grafo.grau = NULL;
    grafo.distancias = NULL;
    grafo.v = malloc(sizeof(Lista) * vertices);
    grafo.grau = malloc(sizeof(int) * vertices);
    grafo.k = 0;
    grafo.arestas = 0;
    for (int i = 0; i < vertices; i++) // inicializando tudo como null
    {
        grafo.v[i].head = grafo.v[i].tail = NULL;
        grafo.grau[i] = 0;
    }
    return grafo;
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

void addEdgeImport(Graph *graph, int u, int v, int weight)
{
    addEdgeToList(&graph->v[u], v, weight);
    graph->arestas += 1;
    graph->grau[u] += 1;
}

Graph importGraph(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    int vertices, arestas_total, k_centros;
    if (fscanf(fp, "%d %d %d", &vertices, &arestas_total, &k_centros) != 3)
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
        addEdgeImport(&graph, u - 1, v - 1, w);
        edges_read++;
    }
    graph.k = k_centros;
    fclose(fp);
    ordenarVizinhos(&graph);
    calcularTodasDistancias(&graph);
    return graph;
}