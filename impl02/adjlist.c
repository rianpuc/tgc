#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct Node
{
    int id;
    struct Node *next;
    struct Node *prev;
} Node;

typedef struct
{
    Node *head;
    Node *tail;
} Lista;

typedef struct
{
    Lista *v;  // lista de adjacencia normal
    Lista *rv; // lista auxiliar pra guardar os antecessores
    int vc;    // quantidade de vertices no grafo
} Graph;

typedef struct
{
    int TD;
    int TT;
    int pai;
} DFS;

typedef struct StackNode
{
    int id;
    struct StackNode *next;
} StackNode;

typedef struct
{
    StackNode *head;
} Stack;

Stack stack;
int tempo = 0;

void criarPilha()
{
    stack.head = malloc(sizeof(StackNode));
    stack.head = NULL;
}

int pilhaVazia()
{
    return stack.head == NULL;
}

void adicionarPilha(int item)
{
    StackNode *stacknode = malloc(sizeof(StackNode));
    stacknode->id = item;
    stacknode->next = stack.head;
    stack.head = stacknode;
}

int removerDaPilha()
{
    StackNode *prev = stack.head;
    stack.head = prev->next;
    free(prev);
}

int verTopoDaPilha()
{
    return stack.head->id;
}

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

Graph initializeGraph(int vertices) // funcao para inicializar o grafo
{
    Graph grafo;
    grafo.vc = vertices;
    grafo.v = malloc(sizeof(Lista) * vertices);
    grafo.rv = malloc(sizeof(Lista) * vertices);
    for (int i = 0; i < vertices; i++) // inicializando tudo como null
    {
        grafo.v[i].head = grafo.v[i].tail = NULL;
        grafo.rv[i].head = grafo.rv[i].tail = NULL;
    }
    return grafo;
}

DFS *initializeDFSTable(int vertices)
{
    DFS *dfs = malloc(sizeof(DFS) * vertices);
    for (int i = 0; i < vertices; i++)
    {
        dfs[i].TD = 0;
        dfs[i].TT = 0;
        dfs[i].pai = -1;
    }
    return dfs;
}

void addVertex(Graph *graph, int origem, int destino)
{
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
}

void DFS_Stack(Graph *graph, DFS *dfs_table)
{
    for (int i = 0; i < graph->vc; i++)
    {
        if (dfs_table[i].TD == 0)
        {
            adicionarPilha(i);
            tempo++;
            dfs_table[i].TD = tempo;
            while (!pilhaVazia())
            {
                int u = verTopoDaPilha();
                Node *vizinhos = graph->v[u].head;
                Node *proximo_vizinho_nao_visitado = NULL;
                while (vizinhos != NULL)
                {
                    if (dfs_table[vizinhos->id].TD == 0)
                    {
                        // printf("{%d, %d} - nao visitada\n", u + 1, vizinhos->id + 1);
                        proximo_vizinho_nao_visitado = vizinhos;
                        break;
                    }
                    vizinhos = vizinhos->next;
                }
                if (proximo_vizinho_nao_visitado != NULL)
                {
                    int v = proximo_vizinho_nao_visitado->id;
                    // printf("vizinho n visitado: %d vai ser oproximo\n", v + 1);
                    dfs_table[v].pai = u;
                    tempo++;
                    dfs_table[v].TD = tempo;
                    adicionarPilha(v);
                    // printf("olhando a pilha: %d\n", verTopoDaPilha() + 1);
                }
                else
                {
                    // printf("%d nao tem nada vou remover\n", u + 1);
                    tempo++;
                    dfs_table[u].TT = tempo;
                    u = removerDaPilha();
                }
            }
        }
    }
}

void printGraph(Graph *graph) // so para debugar
{
    for (int i = 0; i < graph->vc; i++)
    {
        Node *ptr = graph->v[i].head;
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

void printDFS(DFS *dfs_table, int vertices)
{
    for (int i = 0; i < vertices; i++)
    {
        printf("{%d: TD - %d; TT - %d; pai - %d}\n", i + 1, dfs_table[i].TD, dfs_table[i].TT, dfs_table[i].pai + 1);
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
    Graph graph = initializeGraph(vertices);
    for (int i = 0; i < arestas; i++)
    {
        int origem, destino;
        fgets(buffer, 64, fptr);
        sscanf(buffer, "%d %d", &origem, &destino);
        addVertex(&graph, origem - 1, destino - 1);
    }
    int user_input = 1;
    ordenarVizinhos(&graph);
    // printGraph(&graph);
    DFS *dfs_table = initializeDFSTable(vertices);
    // printDFS(dfs_table, vertices);
    criarPilha();
    DFS_Stack(&graph, dfs_table);
    // printDFS(dfs_table, vertices);
    printf("Digite um vertice de 1 ate %d (-1 para sair): ", vertices);
    scanf("%d", &user_input);
    while (user_input > vertices || user_input < -1)
    {
        printf("Favor inserir um numero valido.\n");
        scanf("%d", &user_input);
    }
    while (user_input != -1)
    {
        Node *ptr = graph.v[user_input - 1].head;
        while (ptr != NULL)
        {
            if (dfs_table[ptr->id].pai == user_input - 1)
            {
                printf("{%d, %d} -> Arvore\n", user_input, ptr->id + 1);
            }
            else if (dfs_table[ptr->id].TT > dfs_table[user_input - 1].TT)
            {
                printf("{%d, %d} -> Retorno\n", user_input, ptr->id + 1);
            }
            else
            {
                if (dfs_table[user_input - 1].TD < dfs_table[ptr->id].TD)
                {
                    printf("{%d, %d} -> Avanco\n", user_input, ptr->id + 1);
                }
                else
                {
                    printf("{%d, %d} -> Cruzamento\n", user_input, ptr->id + 1);
                }
            }
            ptr = ptr->next;
        }
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