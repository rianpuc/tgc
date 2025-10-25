#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "graph_generator/graph_generator.h"

typedef struct
{
    int *distancia;
    int *arcos;
    int *pred;
} Dijkstra;

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
}

int selecionarTipoGrafo()
{
    int graph_type = -1;
    printf("Selecione o tipo de grafo:\n");
    printf("1. Conexos.\n");
    printf("2. Completos.\n");
    while (graph_type != 1 && graph_type != 2)
    {
        printf("Opcao: ");
        scanf("%d", &graph_type);
        if (graph_type != 1 && graph_type != 2)
        {
            printf("Opcaoo invalida. Tente 1 ou 2.\n");
            while (getchar() != '\n')
                ;
        }
    }
    return graph_type - 1;
}

int selecionarTamanhoGrafo()
{
    int graph_size = -1;
    printf("Selecione o tamanho do grafo:\n");
    printf("1. 100 vertices.\n");
    printf("2. 1000 vertices.\n");
    printf("3. 5000 vertices.\n");
    printf("4. 10000 vertices.\n");
    while (graph_size < 1 || graph_size > 4)
    {
        printf("Opcao: ");
        scanf("%d", &graph_size);
        if (graph_size < 1 || graph_size > 4)
        {
            printf("Opcao invalida. Tente 1, 2, 3 ou 4.\n");
            while (getchar() != '\n')
                ;
        }
    }
    return graph_size - 1;
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

void dijkstra_sequencial(Graph *grafo, int origem, Dijkstra *res)
{
    bool *visited = malloc(sizeof(bool) * (grafo->vc));
    for (int i = 0; i < grafo->vc; i++)
    {
        res->distancia[i] = INT_MAX;
        res->arcos[i] = INT_MAX;
        res->pred[i] = -1;
        visited[i] = false;
    }
    res->distancia[origem] = 0;
    res->arcos[origem] = 0;
    res->pred[origem] = -1;
    while (!todosVisitados(visited, grafo->vc))
    {
        int atual = menorAtual(res->distancia, visited, grafo->vc);
        if (res->distancia[atual] == INT_MAX || atual == -1)
            break;
        visited[atual] = true;
        Node *ptr = grafo->v[atual].head;
        while (ptr != NULL)
        {
            int distancia = res->distancia[atual] + ptr->weight;
            int arcos = res->arcos[atual] + 1;
            if (visited[ptr->id] != true && res->distancia[ptr->id] > distancia)
            {
                res->distancia[ptr->id] = distancia;
                res->pred[ptr->id] = atual;
            }
            else if (visited[ptr->id] != true && res->distancia[ptr->id] == distancia && arcos < res->arcos[ptr->id])
            {
                res->arcos[ptr->id] = arcos;
                res->pred[ptr->id] = atual;
            }
            ptr = ptr->next;
        }
    }
    free(visited);
}

int imprimirCaminhoRecursivo(Dijkstra *res, int atual)
{
    if (res->pred[atual] == -1)
    {
        printf("%d", atual);
        return 0;
    }
    int num = imprimirCaminhoRecursivo(res, res->pred[atual]);
    printf(" -> %d", atual);
    return num + 1;
}

void imprimirCaminho(Dijkstra *res, int origem, int destino)
{
    if (res->distancia[destino] == INT_MAX)
    {
        printf("Nao existe caminho da origem %d para o destino %d.\n", origem, destino);
    }
    else
    {
        printf("Caminho mais curto de %d para %d (Custo: %d):\n", origem, destino, res->distancia[destino]);
        int tamanho = imprimirCaminhoRecursivo(res, destino);
        printf("\nCaminho de tamanho: %d\n", tamanho);
    }
}

void pesquisarCaminho(Graph *graph)
{
    int origem, destino;
    printf("Digite o vertice de origem: ");
    scanf("%d", &origem);

    if (origem < 0 || origem >= graph->vc)
    {
        printf("Erro: Origem invalida.\n");
        return;
    }

    Dijkstra dijkstra;
    dijkstra.distancia = malloc(sizeof(int) * graph->vc);
    dijkstra.arcos = malloc(sizeof(int) * graph->vc);
    dijkstra.pred = malloc(sizeof(int) * graph->vc);
    dijkstra_sequencial(graph, origem, &dijkstra);

    printf("Dijkstra executado a partir da origem %d.\n", origem);

    while (true)
    {
        printf("Digite o destino (-1 para voltar ao menu): ");
        scanf("%d", &destino);

        if (destino == -1)
        {
            break;
        }

        if (destino < 0 || destino >= graph->vc)
        {
            printf("Erro: Destino invalido.\n");
            continue;
        }

        imprimirCaminho(&dijkstra, origem, destino);
    }
}

void executarBenchmark(Graph *graph)
{
    int repetitions;
    printf("Digite o numero de repeticoes para o benchmark: ");
    scanf("%d", &repetitions);

    if (repetitions <= 0)
    {
        printf("Erro: Numero de repeticoes invalida.\n");
        return;
    }

    double total_time = 0.0;
    printf("Iniciando benchmark (origens aleatorias, %d repeticoes)...\n", repetitions);

    for (int i = 0; i < repetitions; i++)
    {
        int origem = rand() % graph->vc;
        Dijkstra dijkstra;
        dijkstra.distancia = malloc(sizeof(int) * graph->vc);
        dijkstra.arcos = malloc(sizeof(int) * graph->vc);
        dijkstra.pred = malloc(sizeof(int) * graph->vc);

        clock_t start = clock();
        dijkstra_sequencial(graph, origem, &dijkstra);
        clock_t end = clock();

        total_time += ((double)(end - start)) / CLOCKS_PER_SEC;

        free(dijkstra.distancia);
        free(dijkstra.pred);
    }

    double average_time = total_time / repetitions;
    printf("Benchmark concluido.\n");
    printf("-> Tempo total: %f segundos.\n", total_time);
    printf("-> Tempo medio por execucao: %f segundos.\n", average_time);
}

void printMenu()
{
    printf("\t1. Selecionar o tipo de grafo.\n");
    printf("\t2. Selecionar o tamanho do grafo.\n");
    printf("\t3. Benchmark no grafo atual.\n");
    printf("\t4. Pesquisar o menor caminho.\n");
    printf("\t0. Sair do programa.\n");
}

int main()
{
    srand(time(NULL));
    const char *categories[] = {"conexos", "completos"};
    const int sizes[] = {100, 1000, 5000, 10000};
    char filename[256];
    int escolha = -1;
    Graph current_graph;
    bool graph_loaded = false;

    int graph_type_idx = selecionarTipoGrafo();
    int graph_size_idx = selecionarTamanhoGrafo();
    do
    {
        printMenu();
        printf("\nGrafo atual: %s, %d vertices.\n", categories[graph_type_idx], sizes[graph_size_idx]);
        printf("Escolha uma opcao: ");
        scanf("%d", &escolha);
        while (getchar() != '\n')
            ;
        if (!graph_loaded && (escolha == 3 || escolha == 4))
        {
            printf("Carregando grafo... ");
            int vertices = sizes[graph_size_idx];
            const char *category = categories[graph_type_idx];
            snprintf(filename, sizeof(filename), "testing/%s/graph-%d.txt", category, vertices);
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
            graph_type_idx = selecionarTipoGrafo();
            break;

        case 2:
            if (graph_loaded)
            {
                freeGraph(&current_graph);
                graph_loaded = false;
                printf("Grafo anterior desalocado.\n");
            }
            graph_size_idx = selecionarTamanhoGrafo();
            break;

        case 3:
            if (!graph_loaded)
            {
                printf("Erro: Grafo nao carregado.\n");
            }
            else
            {
                executarBenchmark(&current_graph);
            }
            break;

        case 4:
            if (!graph_loaded)
            {
                printf("Erro: Grafo nao carregado.\n");
            }
            else
            {
                pesquisarCaminho(&current_graph);
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