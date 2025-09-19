#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void initializeGraph(int **origem, int **destino, int arestas) // funcao para inicializar o grafo
{
    *origem = malloc(sizeof(int) * arestas);
    *destino = malloc(sizeof(int) * arestas);
}

void swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

int partition(int *origem, int *destino, int low, int high) // particao do quicksort
{
    int pivot = destino[low]; // pegando o pivot arbitrario
    int i = low;
    int j = high;
    while (i < j)
    {
        while (destino[i] <= pivot && i <= high - 1)
        {
            i++;
        }
        while (destino[j] > pivot && j >= low + 1)
        {
            j--;
        }
        if (i < j)
        {
            swap(&destino[i], &destino[j]);
            swap(&origem[i], &origem[j]); // swap do array de origem para manter os predecessores alinhados
        }
    }
    swap(&destino[low], &destino[j]);
    swap(&origem[low], &origem[j]); // swap do array de origem para manter os predecessores alinhados
    return j;
}

void quickSort(int *origem, int *destino, int low, int high)
{
    if (low < high)
    {
        int p = partition(origem, destino, low, high);
        quickSort(origem, destino, low, p - 1);
        quickSort(origem, destino, p + 1, high);
    }
}

int countV(int *origem, int max, int v)
{
    int count = 0;
    for (int i = max; i > 0; i--)
    {
        if (origem[i - 1] < v) // se achei um vertice menor que o meu, ja acabou minhas arestas
        {
            break;
        }
        else
        {
            if (origem[i - 1] == v)
            {
                count++;
            }
        }
    }
    return count;
}

int getGrauSaida(int *origem, int *destino, int vertice)
{
    printf("{ ");
    int start = origem[vertice - 1];
    int end = origem[vertice];
    for (int i = start; i < end; i++)
    {
        printf("%d ", destino[i]);
    }
    printf("}");
    return end - start;
}

int getGrauEntrada(int *origem, int *destino, int vertice)
{
    printf("{ ");
    int start = origem[vertice - 1];
    int end = origem[vertice];
    for (int i = start; i < end; i++)
    {
        printf("%d ", destino[i]);
    }
    printf("}");
    return end - start;
}

void printGraph(int *origem, int *destino, int arestas) // so para debugar
{
    for (int i = 0; i < arestas; i++)
    {
        printf("{%d -> %d} ", origem[i], destino[i]);
    }
    printf("\n");
}

void printForward(int *origem, int *destino, int vertices)
{
    for (int i = 0; i < vertices; i++)
    {
        int start = origem[i];
        int end = origem[i + 1];
        for (int j = start; j < end; j++)
        {
            printf("{ %d -> %d }", i + 1, destino[j]);
        }
        printf("\n");
    }
}

void printPointer(int *origem, int vertices)
{
    for (int i = 0; i < vertices; i++)
    {
        printf(" %d ", origem[i]);
    }
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
    int *origem_arr, *destino_arr;

    // alocando memoria para os arrays do forwardstar e reversestar
    int *pointer = malloc(sizeof(int) * (vertices + 1));
    int *pointer_r = malloc(sizeof(int) * (vertices + 1));
    ///////////////////////////////////////////////////////////////

    // funcao para alocar memoria nos arrays de origem e destino
    initializeGraph(&origem_arr, &destino_arr, arestas);
    ///////////////////////////////////////////////////////////////

    for (int i = 0; i < arestas; i++) // distribuindo os inputs em cada array
    {
        int origem, destino;
        fgets(buffer, 64, fptr);
        sscanf(buffer, "%d %d", &origem, &destino);
        origem_arr[i] = origem;
        destino_arr[i] = destino;
    }

    // alocao de memoria para o arcos de destino (copia do array de destino)
    int *arc_des = malloc(sizeof(int) * arestas);
    memcpy(arc_des, destino_arr, arestas * sizeof(int));
    ////////////////////////////////////////////////////////////////////////

    // fazendo o indice indireto do array forwardstar
    pointer[vertices] = arestas;       // ultima posicao sempre a quantidade de arestas totais
    for (int i = vertices; i > 0; i--) // iterando de tras para frente
    {
        int qtd = countV(origem_arr, arestas, i); // quantidade de vezes que o vertice I aparece = quantidade de arestas
        pointer[i - 1] = pointer[i] - qtd;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // ordenacao do array destino para fazermos o reverse star, junto com ele eu mudo as posicoes da origem tambem
    quickSort(origem_arr, destino_arr, 0, arestas - 1);
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // fazendo o indice indireto do array reversestar
    pointer_r[vertices] = arestas;
    for (int i = vertices; i > 0; i--) // mesma logica do passado
    {
        int qtd = countV(destino_arr, arestas, i);
        pointer_r[i - 1] = pointer_r[i] - qtd;
    }
    /////////////////////////////////////////////////////////////
    printPointer(pointer, vertices);
    printPointer(pointer_r, vertices);
    int user_input = 1;
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
        int grauSaida = getGrauSaida(pointer, arc_des, user_input);
        printf("\nGrau de Saida: %d\n", grauSaida);
        printf("Antecessores:\n");
        int grauEntrada = getGrauEntrada(pointer_r, origem_arr, user_input);
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