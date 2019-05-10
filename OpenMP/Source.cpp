#include <vector>
#include <array>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <omp.h>
#include <deque>
#include <climits>
#include <tuple>

using namespace std;

constexpr auto K = 4;
unsigned int _n = 0;

typedef vector <pair<int, int>> vec;
typedef vector <vec> matrix;
typedef vector <matrix> cube;

enum algoritm {
    Belman_ford = 1,
    Generic,
    SLF,
    LLL
};

string st(const algoritm alg) {
    switch (alg) {
        case 1:
            return "Belman_ford";
        case 2:
            return "Generic";
        case 3:
            return "SLF";
        case 4:
            return "LLL";
    }
}

void write_to_file_raw(matrix &table) {
    ofstream file_out;
    for (int i = 0; i < _n; ++i) {
        file_out.open(to_string(i + 1) + ".node", fstream::out);
        for (int j = 0; j < _n; ++j) {
            file_out << j + 1 << ":" << table[i][j].second << "(" << table[i][j].first << ")\n";
        }
        file_out.close();
    }
}

void print(const vector <array<int, 3>> &g) {
    cout << "  EndNodes\tWeight\n";
    for (const auto &i : g)
        printf("%4d : %4d     %4d\n", i[0], i[1], i[2]);
}

void print(vec &g) {
    cout << "  Distance\tPredecessor\n";
    for (unsigned i = 0; i < _n; ++i)
        printf("%4d : %6d    %d\n", i + 1, g[i].first, g[i].second);
}

void bellman_ford(vec &dp, const vector <array<int, 3>> &g, const int source) {
// dp - vector of distances and predecessors (d, p)
// g - graph (u, v, w)
// source - start node
    for (auto i = 0; i < _n; ++i) {
        dp[i] = make_pair(INT_MAX / 2, -1);       // initialize with big distance and no predecessor
    }

    dp[source].first = 0;                         // distance from source to source is 0

    for (unsigned i = 0; i < g.size(); ++i) {     //
        for (const auto &e : g) {                 // for each edge in graph
            const auto u = e[0] - 1;
            const auto v = e[1] - 1;
            const auto w = e[2];
            if (dp[u].first + w < dp[v].first) {  // relaxation
                dp[v].first = dp[u].first + w;    // update distance
                dp[v].second = u + 1;             // update predecessor
            }
        }
    }
}

void generic(vec &table, const vector <array<int, 3>> &g, const int source) {
// dp - vector of distances and predecessors (d, p)
// g - graph (u, v, w)
// source - start node

    for (auto i = 0; i < _n; ++i){
        table[i] = make_pair(INT_MAX / 2, -1);
    }

    table[source].first = 0;
    vector<int> Q;
    vector<bool> is(_n, false);
    Q.push_back(source + 1);
    is[source] = true;

    while (!Q.empty()) {
        int u = Q.back();
        Q.pop_back();
        is[u - 1] = false;
        int v;
        for (const auto &j : g) {
            if (u == j[0])
                v = j[1];
            else if (u == j[1])
                v = j[0];
            else continue;
            const auto u1 = u - 1;
            const auto v1 = v - 1;
            const auto w = j[2];
            if (table[u1].first + w < table[v1].first) {
                table[v1].first = table[u1].first + w;
                table[v1].second = u;
                if (!is[v1]) {
                    Q.push_back(v);
                    is[v1] = true;
                }
            }
        }
    }
}

void slf(vec &table, const vector <array<int, 3>> &g, const int source) {
// dp - vector of distances and predecessors (d, p)
// g - graph (u, v, w)
// source - start node

    for (auto i = 0; i < _n; ++i) {
        table[i] = make_pair(INT_MAX / 2, -1);
    }

    table[source].first = 0;
    deque<int> Q;
    vector<bool> is(_n, false);
    Q.push_back(source + 1);
    is[source] = true;

    while (!Q.empty()) {
        int u = Q.back();
        Q.pop_back();
        is[u - 1] = false;
        int v;
        for (const auto &j : g) {
            if (u == j[0])
                v = j[1];
            else if (u == j[1])
                v = j[0];
            else continue;
            const auto u1 = u - 1;
            const auto v1 = v - 1;
            const auto w = j[2];
            if (table[u1].first + w < table[v1].first) {
                table[v1].first = table[u1].first + w;
                table[v1].second = u;
                if (!is[v1]) {
                    if (!Q.empty() && table[v1].first < Q.back())
                        Q.push_front(v);
                    else
                        Q.push_back(v);
                    is[v1] = true;
                }
            }
        }
    }
}

void lll(vec &table, const vector <array<int, 3>> &g, const int source) {
// dp - vector of distances and predecessors (d, p)
// g - graph (u, v, w)
// source - start node

    for (auto i = 0; i < _n; ++i) {
        table[i] = make_pair(INT_MAX / 2, -1);
    }
    
    table[source].first = 0;
    deque<int> Q;
    vector<bool> is(_n, false);
    Q.push_back(source + 1);
    is[source] = true;
    int sum = 0;

    while (!Q.empty()) {
        const auto u = Q.back();
        const auto u1 = u - 1;
        Q.pop_back();
        sum -= table[u1].first;
        is[u1] = false;
        int v;

        for (const auto &j : g) {
            if (u == j[0])
                v = j[1];
            else if (u == j[1])
                v = j[0];
            else continue;
            const auto v1 = v - 1;
            const auto w = j[2];
            if (table[u1].first + w < table[v1].first) {
                table[v1].first = table[u1].first + w;
                table[v1].second = u;
                if (!is[v1]) {
                    Q.push_back(v);
                    sum += table[v1].first;
                    const double avg = (sum * 1.01) / Q.size();
                    v = Q.front();
                    while (table[v - 1].first > avg) {
                        Q.pop_front();
                        Q.push_back(v);
                        v = Q.front();
                    }
                    is[v1] = true;
                }
            }
        }
    }
}

tuple<double, int, int>
routing_table(matrix &table, const vector <array<int, 3>> &graph, const algoritm alg, int threads) {
    decltype(bellman_ford) *fun;

    if (alg == Belman_ford)
        fun = &bellman_ford;
    else if (alg == Generic)
        fun = &generic;
    else if (alg == SLF)
        fun = &slf;
    else if (alg == LLL)
        fun = &lll;
    else
        return make_tuple(0.0, 0, 0);


    int num_threads, num_cores;

    const auto t1 = chrono::high_resolution_clock::now();
    #pragma omp parallel num_threads(threads)
    {
        #pragma omp for schedule(guided)
        for (int i = 0; i < _n; i++)
            fun(table[i], graph, i);
        #pragma omp master
        {
            num_threads = omp_get_num_threads();
            num_cores = omp_get_num_procs();
        }
    }
    const auto t2 = chrono::high_resolution_clock::now();
    chrono::duration<double, std::milli> fp_ms = t2 - t1;

    return make_tuple(fp_ms.count() / 1000.0, num_cores, num_threads);
}

bool check(cube &table) {
    for (unsigned i = 0; i < _n; ++i) {
        for (unsigned j = 0; j < _n; ++j) {
            for (unsigned k = 1; k < K; k++)
                if (table[0][i][j].first != table[k][i][j].first) {
                    cout << k << ": [ " << i << " ] [ " << j << " ]\n";
                    return false;
                }
        }
    }
    return true;
}

int main(int argc, char **argv) // argv = [name, alg, threads]
{

    vector <array<int, 3>> graph;
    ifstream file_in(string(argv[1]), fstream::in);
    file_in >> _n;
    int a, b, c;
    while (file_in >> a >> b >> c) {
        graph.emplace_back(array < int, 3 > {a, b, c});
    }
    file_in.close();

    cube table(K, matrix(_n, vec(_n)));
    int alg = atoi(argv[2]);
    int threads = atoi(argv[3]);

    auto result = routing_table(table[0], graph, algoritm(alg), threads);

    cout<<get<0>(result)<<endl;

    // ofstream file_out("time.txt", fstream::out);
    // file_out << get<0>(result);
    // file_out.close();

    //write_to_file_raw(table[0]);

    return 0;
}
