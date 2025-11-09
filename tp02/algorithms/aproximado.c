#include "algorithms.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "../graph_generator/graph_generator.h"
#define INF (INT_MAX / 2)

int algoritmoAproximado(Graph *graph)
{
    int V = graph->vc;
    int k = graph->k;
    int **distancias = graph->distancias;

    int *centros = malloc(k * sizeof(int));
    int *distParaCentroProximo = malloc(V * sizeof(int));
    bool *ehCentro = calloc(V, sizeof(bool));

    int primeiroCentro = 0;
    centros[0] = primeiroCentro;
    ehCentro[primeiroCentro] = true;

    for (int i = 0; i < V; i++)
    {
        distParaCentroProximo[i] = distancias[i][primeiroCentro];
    }

    for (int i = 1; i < k; i++)
    {
        int maxDist = -1;
        int proximoCentro = -1;
        for (int v = 0; v < V; v++)
        {
            if (!ehCentro[v] && distParaCentroProximo[v] > maxDist)
            {
                maxDist = distParaCentroProximo[v];
                proximoCentro = v;
            }
        }
        centros[i] = proximoCentro;
        ehCentro[proximoCentro] = true;
        for (int v = 0; v < V; v++)
        {
            if (distancias[v][proximoCentro] < distParaCentroProximo[v])
            {
                distParaCentroProximo[v] = distancias[v][proximoCentro];
            }
        }
    }

    int raioAproximado = 0;
    for (int v = 0; v < V; v++)
    {
        if (distParaCentroProximo[v] > raioAproximado)
        {
            raioAproximado = distParaCentroProximo[v];
        }
    }

    free(centros);
    free(distParaCentroProximo);
    free(ehCentro);

    return raioAproximado;
}