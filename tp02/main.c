#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "algorithms/algorithms.h"
#include "graph_generator/graph_generator.h"

const double TIMEOUT_LIMIT = 600.0; // 10 minutos timeout
const int raios_otimos[40] = {
    127, 98, 93, 74, 48, // 1-5
    84, 64, 55, 37, 20,  // 6-10
    59, 51, 35, 26, 18,  // 11-15
    47, 39, 28, 18, 13,  // 16-20
    40, 38, 22, 15, 11,  // 21-25
    38, 32, 18, 13, 9,   // 26-30
    30, 29, 15, 11, 30,  // 31-35
    27, 15, 29, 23, 13   // 36-40
};

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
    printf("\t5. EXECUTAR TODOS OS 40 TESTES (Modo Batch).\n");
    printf("\t0. Sair do programa.\n");
}

void rodarTodosOsTestes()
{
    char filename[256];
    Graph graph;

    // Defina seu limite de tempo em segundos (ex: 300s = 5 minutos)
    const double TIMEOUT_LIMIT = 300.0;
    bool exato_timeout_atingido = false;

    // Imprime o cabecalho da tabela (formato CSV)
    // CSV (Separado por Ponto-e-Virgula) e facil de colar no Excel/Google Sheets
    printf("Instancia;V;k;RaioOtimo;RaioAprox;TempoAprox(s);RaioExato;TempoExato(s)\n");

    for (int i = 1; i <= 40; i++)
    {
        snprintf(filename, sizeof(filename), "testing/pmed%d.txt", i);
        graph = importGraph(filename);
        if (graph.vc == 0)
        {
            fprintf(stderr, "Erro ao carregar %s\n", filename);
            continue;
        }

        int raio_otimo = raios_otimos[i - 1];

        clock_t start_aprox = clock();
        int raio_aprox = algoritmoAproximado(&graph);
        clock_t end_aprox = clock();
        double tempo_aprox = (double)(end_aprox - start_aprox) / CLOCKS_PER_SEC;

        int raio_exato = -1;
        double tempo_exato = -1.0;

        if (exato_timeout_atingido)
        {
            // Pula o teste exato
        }
        else
        {
            clock_t start_exato = clock();
            raio_exato = algoritmoExato(&graph);
            clock_t end_exato = clock();
            tempo_exato = (double)(end_exato - start_exato) / CLOCKS_PER_SEC;

            if (tempo_exato > TIMEOUT_LIMIT)
            {
                exato_timeout_atingido = true;
                printf("### TIMEOUT ATINGIDO NO GRAFO %d (%.2fs) ###\n", i, tempo_exato);
            }
        }

        printf("pmed%d;%d;%d;%d;", i, graph.vc, graph.k, raio_otimo);
        printf("%d;%.7f;", raio_aprox, tempo_aprox);

        if (tempo_exato == -1.0)
        {
            printf("TIMEOUT;TIMEOUT\n");
        }
        else
        {
            printf("%d;%.7f\n", raio_exato, tempo_exato);
        }
        freeGraph(&graph);
    }
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
                printf("Tempo de execucao: %.7f segundos\n", tempo);
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
                printf("Tempo de execucao: %.7f segundos\n", tempo);
            }
            else
            {
                printf("Erro: Grafo nao carregado.\n");
            }
            break;
        case 5:
            printf("--- INICIANDO TESTE EM LOTE (40 INSTANCIAS) ---\n");
            if (graph_loaded)
            {
                printf("Aviso: Desalocando grafo interativo atual...\n");
                freeGraph(&current_graph);
                graph_loaded = false;
            }
            rodarTodosOsTestes();
            printf("--- TESTE EM LOTE CONCLUIDO ---\n");
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