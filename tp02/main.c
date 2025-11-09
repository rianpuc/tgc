#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "algorithms/algorithms.h"
#include "graph_generator/graph_generator.h"

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
    if (graph->distancias != NULL)
    {
        for (int i = 0; i < graph->vc; i++)
        {
            if (graph->distancias[i] != NULL)
            {
                free(graph->distancias[i]);
            }
        }
        free(graph->distancias);
    }
    graph->v = NULL;
    graph->grau = NULL;
    graph->distancias = NULL;
    graph->vc = 0;
}

void printInfo(Graph *grafo)
{
    printf("\tV: %d\n\tE: %d\n\tk: %d\n------", grafo->vc, grafo->arestas, grafo->k);
}

int selecionarGrafo()
{
    int graph_id = -1;
    printf("Selecione o grafo:\n");
    printf("Entre 1 a 40.\n");
    while (graph_id < 1 || graph_id > 40)
    {
        printf("Opcao: ");
        scanf("%d", &graph_id);
        if (graph_id < 1 || graph_id > 40)
        {
            printf("Opcao invalida. Tente um numero entre 1 e 40.\n");
            while (getchar() != '\n')
                ;
        }
    }
    return graph_id;
}

void printMenu()
{
    printf("\t1. Selecionar outro grafo.\n");
    printf("\t2. Mostrar informacoes (V, E, k).\n");
    printf("\t3. Rodar Algoritmo EXATO.\n");
    printf("\t4. Rodar Algoritmo APROXIMADO.\n");
    printf("\t0. Sair do programa.\n");
}

int main()
{
    srand(time(NULL));
    char filename[256];
    int escolha = -1;
    Graph current_graph;
    bool graph_loaded = false;
    int graph_idx = selecionarGrafo();
    do
    {
        printMenu();
        printf("\nGrafo atual: %d\n", graph_idx);
        printf("Escolha uma opcao: ");
        scanf("%d", &escolha);
        while (getchar() != '\n')
            ;
        if (!graph_loaded)
        {
            printf("Carregando grafo... ");
            snprintf(filename, sizeof(filename), "testing/pmed%d.txt", graph_idx);
            current_graph = importGraph(filename);
            graph_loaded = true;
            printf("Concluido.\n");
        }
        switch (escolha)
        {
        case 1:
            if (graph_loaded)
            {
                freeGraph(&current_graph);
                graph_loaded = false;
                printf("Grafo anterior desalocado.\n");
            }
            graph_idx = selecionarGrafo();
            break;

        case 2:
            if (!graph_loaded)
            {
                printf("Erro: Grafo nao carregado.\n");
            }
            else
            {
                printInfo(&current_graph);
            }
            break;
        case 3:
            if (graph_loaded)
            {
                printf("Executando Algoritmo Exato (pode demorar)...\n");
                clock_t start = clock();
                int raioExato = algoritmoExato(&current_graph);
                clock_t end = clock();
                double tempo = (double)(end - start) / CLOCKS_PER_SEC;
                printf("--- Resultado (Exato) ---\n");
                printf("Raio encontrado: %d\n", raioExato);
                printf("Tempo de execucao: %.8f segundos\n", tempo);
            }
            else
            {
                printf("Erro: Grafo nao carregado.\n");
            }
            break;
        case 4:
            if (graph_loaded)
            {
                printf("Executando Algoritmo Aproximado\n");
                clock_t start = clock();
                int raioAproximado = algoritmoAproximado(&current_graph);
                clock_t end = clock();
                double tempo = (double)(end - start) / CLOCKS_PER_SEC;
                printf("--- Resultado (Aproximado) ---\n");
                printf("Raio encontrado: %d\n", raioAproximado);
                printf("Tempo de execucao: %.8f segundos\n", tempo);
            }
            else
            {
                printf("Erro: Grafo nao carregado.\n");
            }
            break;
        case 0:
            printf("Saindo do programa...\n");
            break;

        default:
            printf("Opcao invalida. Tente novamente.\n");
            break;
        }

        printf("\n");
    } while (escolha != 0);
    if (graph_loaded)
    {
        freeGraph(&current_graph);
        printf("Grafo desalocado.\n");
    }
    return 0;
}