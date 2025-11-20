#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <fstream>
#include <filesystem>
#include <cstring>
#include <iomanip>

using namespace std;
namespace fs = std::filesystem;

const int INF = 1e9;

// --- ESTRUTURAS ---
struct Edge
{
    int to;
    int cap;
    int flow;
    int rev;
    int id_original;
};

// --- GLOBAIS ---
vector<vector<Edge>> adj;
vector<int> level;
vector<int> ptr;
int N, M;

// --- FUNÇÕES DO ALGORITMO ---

void add_edge(int from, int to, int id)
{
    Edge a = {to, 1, 0, (int)adj[to].size(), id};
    Edge b = {from, 0, 0, (int)adj[from].size(), -1};
    adj[from].push_back(a);
    adj[to].push_back(b);
}

bool bfs(int s, int t)
{
    fill(level.begin(), level.end(), -1);
    level[s] = 0;
    queue<int> q;
    q.push(s);
    while (!q.empty())
    {
        int v = q.front();
        q.pop();
        for (auto &edge : adj[v])
        {
            if (edge.cap - edge.flow > 0 && level[edge.to] == -1)
            {
                level[edge.to] = level[v] + 1;
                q.push(edge.to);
            }
        }
    }
    return level[t] != -1;
}

int dfs(int v, int t, int pushed)
{
    if (pushed == 0)
        return 0;
    if (v == t)
        return pushed;
    for (int &cid = ptr[v]; cid < adj[v].size(); ++cid)
    {
        auto &edge = adj[v][cid];
        int tr = edge.to;
        if (level[v] + 1 != level[tr] || edge.cap - edge.flow == 0)
            continue;
        int push = dfs(tr, t, min(pushed, edge.cap - edge.flow));
        if (push == 0)
            continue;
        edge.flow += push;
        adj[tr][edge.rev].flow -= push;
        return push;
    }
    return 0;
}

int dinic(int s, int t)
{
    int flow = 0;
    while (bfs(s, t))
    {
        fill(ptr.begin(), ptr.end(), 0);
        while (int pushed = dfs(s, t, INF))
        {
            flow += pushed;
        }
    }
    return flow;
}

void file_print_path(ofstream &out, int u, int t, vector<int> &path)
{
    path.push_back(u);
    if (u == t)
    {
        for (size_t i = 0; i < path.size(); i++)
        {
            out << path[i] << (i == path.size() - 1 ? "" : " -> ");
        }
        out << endl;
        return;
    }
    for (auto &edge : adj[u])
    {
        if (edge.cap == 1 && edge.flow == 1)
        {
            edge.flow = 0;
            file_print_path(out, edge.to, t, path);
            return;
        }
    }
}

void processar_arquivo(string filepath, ofstream &log_file)
{
    adj.clear();
    level.clear();
    ptr.clear();
    N = 0;
    M = 0;

    ifstream file(filepath);
    if (!file.is_open())
    {
        cerr << "Erro ao abrir: " << filepath << endl;
        return;
    }

    file >> N >> M;

    adj.assign(N + 1, vector<Edge>());
    level.resize(N + 1);
    ptr.resize(N + 1);

    int u, v;
    for (int i = 0; i < M; i++)
    {
        file >> u >> v;
        add_edge(u, v, i);
    }
    file.close();

    int s = 0;
    int t = N - 1;

    clock_t start = clock();
    int max_paths = dinic(s, t);
    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;

    cout << filepath << ";" << N << ";" << M << ";" << max_paths << ";" << fixed << setprecision(6) << time_spent << endl;

    log_file << "=== " << filepath << " ===" << endl;
    log_file << "Caminhos encontrados: " << max_paths << endl;
    for (int i = 0; i < max_paths; i++)
    {
        log_file << "Caminho " << i + 1 << ": ";
        vector<int> path;
        file_print_path(log_file, s, t, path);
    }
    log_file << "----------------------------------------" << endl;
}

int main(int argc, char *argv[])
{
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    vector<string> pastas = {
        "testing/completos",
        "testing/conexos"};

    ofstream log_file("saida_detalhada_caminhos.txt");

    cout << "Arquivo;Vertices;Arestas;Caminhos;Tempo(s)" << endl;

    for (const auto &pasta : pastas)
    {
        if (!fs::exists(pasta))
        {
            cerr << "AVISO: Pasta nao encontrada: " << pasta << endl;
            continue;
        }

        for (const auto &entry : fs::directory_iterator(pasta))
        {
            string path_string = entry.path().string();

            if (path_string.find(".txt") != string::npos)
            {
                processar_arquivo(path_string, log_file);
            }
        }
    }

    log_file.close();
    cout << "\nProcessamento concluido!" << endl;
    cout << "Listagem detalhada salva em 'saida_detalhada_caminhos.txt'." << endl;

    return 0;
}