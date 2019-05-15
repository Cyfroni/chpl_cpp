int sum = 0;
#pragma omp target device(gpu) teams distribute \
parallel for reduction(+:sum)
  for( int j = 0; j < n; j++) {
    sum += j;
  }
