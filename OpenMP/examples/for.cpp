#pragma omp parallel for
{
    for (int i = 0; i < 10; i++) {
        cout << i << endl;
    }
}


int sum = 0;

#pragma omp parallel for reduction(+:sum)
{
    for (int i = 0; i < 10; i++) {
        sum += i;
    }
}
