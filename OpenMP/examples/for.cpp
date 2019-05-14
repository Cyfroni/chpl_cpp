#include <iostream>
#include <omp.h>

int main() {

    #pragma omp parallel for
    {
        for (int i = 0; i < 10; i++) {
            cout << i << endl;
        }
    }
}
