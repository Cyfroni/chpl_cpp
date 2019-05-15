int data[N];

#pragma omp target device(d1)
#pragma omp parallel for
for (int i = 0; i < N; i++)
   data[i] *= 2;

// Do something else

#pragma omp target device(d1)
#pragma omp parallel for
for (int i = 0; i < N; i++)
   data[i] += 5;

/////

int data[N];

#pragma omp target data map(tofrom: data)
{
  #pragma omp target device(d1)
  #pragma omp for
  for (int i = 0; i < N; i++)
     data[i] *= 2;

  #pragma omp target update from(data)

  // Do something else

  #pragma omp target device(d1)
  #pragma omp for
  for (int i = 0; i < N; i++)
     data[i] += 5;
}
