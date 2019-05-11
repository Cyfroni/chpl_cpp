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

void bellman_ford(vec &dp, const vector <array<int, 3>> &g, const int s) {
// dp - vector of distances and predecessors (d, p)
// g - graph (i, j, a)
// s - start node

    for (auto x = 0; x < _n; ++x) {
        dp[x] = make_pair(INT_MAX / 2, -1);       // initialize with big distance
    }                                             // and no predecessor

    dp[s].first = 0;                              // distance from source to source is 0

    for (unsigned x = 0; x < g.size(); ++x) {
        for (const auto &e : g) {                 // for each edge in graph
            const auto i = e[0] - 1;              // index adjust
            const auto j = e[1] - 1;              // index adjust
            const auto a = e[2];
            if (dp[i].first + a < dp[j].first) {  // relaxation
                dp[j].first = dp[j].first + a;    // update distance
                dp[j].second = j + 1;             // update predecessor
            }
        }
    }
}

void generic(vec &dp, const vector <array<int, 3>> &g, const int s) {
// dp - vector of distances and predecessors (d, p)
// g - graph (i, j, a)
// s - start node

    for (auto x = 0; x < _n; ++x) {
        dp[x] = make_pair(INT_MAX / 2, -1);       // initialize with big distance
    }                                             // and no predecessor

    dp[s].first = 0;                              // distance from source to source is 0
    deque<int> V;
    vector<bool> is(_n, false);                   // table of existance in V
    V.push_back(s);                               // Initialize queue with source node
    is[s] = true;                                 // mark it in the table

    while (!V.empty()) {
        int _i = V.front();                       // take from the top
        V.pop_front();
        is[_i] = false;                           // mark it

        for (const auto &e : g) {
            const auto i = e[0] - 1;
            const auto j = e[1] - 1;
            const auto a = e[2];
            if (i != _i || j==s) continue;        // process only edges from i
                                                  // and ommit edges to source
            if (dp[i].first + a < dp[j].first) {  // relaxation
                dp[j].first = dp[i].first + a;    // update distance
                dp[j].second = i;                 // update predecessor
                if (!is[j]) {
                    V.push_back(j);               // add v to V if it's not there already.
                    is[j] = true;
                }
            }
        }
    }
}

void slf(vec &dp, const vector <array<int, 3>> &g, const int s) {
// dp - vector of distances and predecessors (d, p)
// g - graph (i, j, a)
// s - start node

    for (auto x = 0; x < _n; ++x) {
        dp[x] = make_pair(INT_MAX / 2, -1);       // initialize with big distance
    }                                             // and no predecessor

    dp[s].first = 0;                              // distance from source to source is 0
    deque<int> V;
    vector<bool> is(_n, false);                   // table of existance in V
    V.push_back(s);                               // Initialize queue with source node
    is[s] = true;                                 // mark it in the table

    while (!V.empty()) {
        int _i = V.front();                       // take from the top
        V.pop_front();
        is[_i] = false;                           // mark it

        for (const auto &e : g) {
            const auto i = e[0] - 1;
            const auto j = e[1] - 1;
            const auto a = e[2];
            if (i != _i || j==s) continue;        // process only edges from i
                                                  // and ommit edges to source
            if (dp[i].first + a < dp[j].first) {  // relaxation
                dp[j].first = dp[i].first + a;    // update distance
                dp[j].second = i;                 // update predecessor
                if (!is[j]) {
                    if (!V.empty() &&             // SLF rule for insertion
                        dp[j].first <= dp[V.front()].first){
                        V.push_front(j);
                    }
                    else {
                        V.push_back(j);
                    }
                    is[j] = true;
                }
            }
        }
    }
}

void lll(vec &dp, const vector <array<int, 3>> &g, const int s) {
// dp - vector of distances and predecessors (d, p)
// g - graph (i, j, a)
// s - start node

    for (auto x = 0; x < _n; ++x) {
        dp[x] = make_pair(INT_MAX / 2, -1);       // initialize with big distance
    }                                             // and no predecessor

    dp[s].first = 0;                              // distance from source to source is 0
    deque<int> V;
    vector<bool> is(_n, false);                   // table of existance in V
    V.push_back(s);                               // Initialize queue with source node
    is[s] = true;                                 // mark it in the table

    while (!V.empty()) {
        int _i = V.front();                       // take from the top
        V.pop_front();
        is[_i] = false;                           // mark it

        for (const auto &e : g) {
            const auto i = e[0] - 1;
            const auto j = e[1] - 1;
            const auto a = e[2];
            if (i != _i || j==s) continue;        // process only edges from i
                                                  // and ommit edges to source
            if (dp[i].first + a < dp[j].first) {  // relaxation
                dp[j].first = dp[i].first + a;    // update distance
                dp[j].second = i;                 // update predecessor
                if (!is[j]) {
                    V.push_back(j);               // insertion like in generic alg.
                    is[j] = true;
                    var sum = 0;
                    for (int x=0; x < V.size(); ++x){
                      sum += dp[j].first;         // sum of all node labels in V
                    }
                    double c = sum * / V.size();  // c - average node label in V
                    var f = V.front();
                    while (dp[f].first > c) {     // LLL rule for rearrangement
                        V.pop_front();
                        V.push_back(v);
                        f = V.front();
                    }
                }
            }
        }
    }



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
