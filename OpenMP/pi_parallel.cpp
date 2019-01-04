#include <iostream>
#include <chrono>
#include <omp.h>

const long num_steps = 1000000000;
const double step = 1.0 / (double) num_steps;
const long threads = 4

int main() {
    const auto t1 = chrono::high_resolution_clock::now();

    double pi_sum = 0.0;
    #pragma omp parallel num_threads(threads)
    {
        #pragma omp for reduction(+:pi_sum)
        for (int i = 0; i < num_steps; i++) {
            double x = (i + 0.5) * step;
            pi_sum += 4.0 / (1.0 + x * x);
        }
    }
    double pi = pi_sum * step;

    const auto t2 = chrono::high_resolution_clock::now();
    chrono::duration<double, std::milli> fp_ms = t2 - t1;

    cout << "threads:\t" << threads
         << "\ntime:\t\t" << fp_ms / 1000.0
         << "\npi:\t\t\t" << pi;
}