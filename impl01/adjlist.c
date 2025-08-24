#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node
{
    int id;
    struct Node *next;
} Node;

typedef struct
{
    Node **v;  // lista de adjacencia normal
    Node **rv; // lista auxiliar pra guardar os antecessores
    int vc;    // quantidade de vertices no grafo
} Graph;

Graph initializeGraph(int vertices) // funcao para inicializar o grafo
{
    Graph grafo;
    grafo.vc = vertices;
    grafo.v = malloc(sizeof(Node *) * vertices);
    grafo.rv = malloc(sizeof(Node *) * vertices);
    for (int i = 0; i < vertices; i++) // inicializando tudo como null
    {
        grafo.v[i] = NULL;
        grafo.rv[i] = NULL;
    }
    return grafo;
}

void addVertex(Graph *graph, int origem, int destino)
{
    Node *dest = malloc(sizeof(Node));
    Node *orig = malloc(sizeof(Node));
    dest->id = destino;
    orig->id = origem;
    dest->next = orig->next = NULL;
    // adicionando o vizinho na lista de adjacencia
    if (graph->v[origem] == NULL)
    {
        graph->v[origem] = dest;
    }
    else
    {
        dest->next = graph->v[origem];
        graph->v[origem] = dest;
    }
    // adicionando a origem como predecessor na lista auxiliar para facilitar a busca
    if (graph->rv[destino] == NULL)
    {
        graph->rv[destino] = orig;
    }
    else
    {
        orig->next = graph->rv[destino];
        graph->rv[destino] = orig;
    }
}

int getGrauSaida(Graph *graph, int vertice)
{
    int sum = 0;
    printf("{ ");
    Node *ptr = graph->v[vertice];
    while (ptr != NULL)
    {
        sum++;
        printf("%d ", ptr->id + 1);
        ptr = ptr->next;
    }
    printf("}");
    return sum;
}

int getGrauEntrada(Graph *graph, int vertice)
{
    int sum = 0;
    printf("{ ");
    Node *ptr = graph->rv[vertice];
    while (ptr != NULL)
    {
        sum++;
        printf("%d ", ptr->id + 1);
        ptr = ptr->next;
    }
    printf("}");
    return sum;
}

void printGraph(Graph *graph) // so para debugar
{
    for (int i = 0; i < graph->vc; i++)
    {
        Node *ptr = graph->v[i];
        printf("Vertice: %d, Vizinhos:\n", i);
        while (ptr != NULL)
        {
            printf("%d ", ptr->id);
            ptr = ptr->next;
        }
        printf("\n");
    }
    printf("\n");
}

int main(int argc, char **argv)
{
    FILE *fptr;
    char buffer[64];
    printf("Digite o NOME do arquivo .txt a ser lido (max 64 caracteres): ");
    fgets(buffer, 64, stdin);
    size_t tamanho = strlen(buffer);
    char *arquivo = malloc(sizeof(char) * (tamanho + 4));
    memcpy(arquivo, buffer, strlen(buffer));
    arquivo[tamanho - 1] = '.';
    arquivo[tamanho] = 't';
    arquivo[tamanho + 1] = 'x';
    arquivo[tamanho + 2] = 't';
    arquivo[tamanho + 3] = '\0';
    fptr = fopen(arquivo, "r");
    if (fptr == NULL)
    {
        printf("Erro ao encontrar o arquivo.\n");
        return -1;
    }
    int vertices, arestas;
    fgets(buffer, 64, fptr);
    sscanf(buffer, "%d %d", &vertices, &arestas);
    Graph graph = initializeGraph(vertices);
    for (int i = 0; i < arestas; i++)
    {
        int origem, destino;
        fgets(buffer, 64, fptr);
        sscanf(buffer, "%d %d", &origem, &destino);
        addVertex(&graph, origem - 1, destino - 1);
    }
    int user_input = 1;
    // printGraph(&graph);
    printf("Digite um vertice de 1 ate %d (-1 para sair): ", vertices);
    scanf("%d", &user_input);
    while (user_input > vertices || user_input < -1)
    {
        printf("Favor inserir um numero valido.\n");
        scanf("%d", &user_input);
    }
    while (user_input != -1)
    {
        printf("Sucessores:\n");
        int grauSaida = getGrauSaida(&graph, user_input - 1);
        printf("\nGrau de Saida: %d\n", grauSaida);
        printf("Antecessores:\n");
        int grauEntrada = getGrauEntrada(&graph, user_input - 1);
        printf("\nGrau de Entrada: %d\n", grauEntrada);
        printf("Digite um vertice de 1 ate %d (-1 para sair): ", vertices);
        scanf("%d", &user_input);
        while (user_input > vertices || user_input < -1)
        {
            printf("Favor inserir um numero valido.\n");
            scanf("%d", &user_input);
        }
    }
    fclose(fptr);
    return 0;
}