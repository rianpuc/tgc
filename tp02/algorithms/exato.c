#include "algorithms.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "../graph_generator/graph_generator.h"
#define INF (INT_MAX / 2)

int calcularRaio(int V, int k, int *centros, int **distancias, int melhorRaioGeral)
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
        if (maxRaio > melhorRaioGeral)
        {
            return maxRaio;
        }
    }
    return maxRaio;
}

void encontrarMelhorRaio(int V, int k, int **distancias,
                         int *centros, int index, int proximoVertice, int *melhorRaio,
                         clock_t start_time, double limit, bool *timeout_flag)
{
    if (*timeout_flag)
    {
        return;
    }
    if (index == k)
    {
        static unsigned long long check_counter = 0;
        check_counter++;
        if (check_counter % 1000000 == 0)
        {
            clock_t now = clock();
            if ((double)(now - start_time) / CLOCKS_PER_SEC > limit)
            {
                *timeout_flag = true;
            }
        }
        if (*timeout_flag)
        {
            return;
        }
        int raioAtual = calcularRaio(V, k, centros, distancias, *melhorRaio);
        if (raioAtual < *melhorRaio)
        {
            *melhorRaio = raioAtual;
        }
        return;
    }

    for (int i = proximoVertice; i < V; i++)
    {
        if (*timeout_flag)
        {
            return;
        }
        centros[index] = i;
        encontrarMelhorRaio(V, k, distancias, centros, index + 1, i + 1, melhorRaio, start_time, limit, timeout_flag);
    }
}

int algoritmoExato(Graph *graph, clock_t start_time, double limit, bool *timeout_flag)
{
    int V = graph->vc;
    int k = graph->k;
    int **distancias = graph->distancias;

    int *centros = malloc(k * sizeof(int));
    int melhorRaio = INF;
    *timeout_flag = false;

    encontrarMelhorRaio(V, k, distancias, centros, 0, 0, &melhorRaio, start_time, limit, timeout_flag);

    free(centros);
    return melhorRaio;
}