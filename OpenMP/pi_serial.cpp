#include <iostream>

using namespace std;

const long num_steps = 1000000000;
const double step = 1.0 / (double) num_steps;

int main() {

    double pi_sum = 0.0;
    for (int i = 0; i < num_steps; i++) {
        double x = (i + 0.5) * step;
        pi_sum += 4.0 / (1.0 + x * x);
    }
    double pi = pi_sum * step;

    cout << pi << endl;
}
