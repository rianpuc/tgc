#include "algorithms.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "../graph_generator/graph_generator.h"
#define INF (INT_MAX / 2)

int calcularRaio(int V, int k, int *centros, int **distancias)
{
    int maxRaio = 0;

    for (int i = 0; i < V; i++)
    {
        int distCentroMaisProximo = INF;

        for (int j = 0; j < k; j++)
        {
            int centroId = centros[j];
            if (distancias[i][centroId] < distCentroMaisProximo)
            {
                distCentroMaisProximo = distancias[i][centroId];
            }
        }

        if (distCentroMaisProximo > maxRaio)
        {
            maxRaio = distCentroMaisProximo;
        }
    }
    return maxRaio;
}

void encontrarMelhorRaio(int V, int k, int **distancias,
                         int *centros, int index, int proximoVertice, int *melhorRaio)
{

    if (index == k)
    {
        int raioAtual = calcularRaio(V, k, centros, distancias);
        if (raioAtual < *melhorRaio)
        {
            *melhorRaio = raioAtual;
        }
        return;
    }

    for (int i = proximoVertice; i < V; i++)
    {
        centros[index] = i;
        encontrarMelhorRaio(V, k, distancias, centros, index + 1, i + 1, melhorRaio);
    }
}

int algoritmoExato(Graph *graph)
{
    int V = graph->vc;
    int k = graph->k;
    int **distancias = graph->distancias;

    int *centros = malloc(k * sizeof(int));
    int melhorRaio = INF;

    encontrarMelhorRaio(V, k, distancias, centros, 0, 0, &melhorRaio);

    free(centros);
    return melhorRaio;
}