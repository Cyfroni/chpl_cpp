#pragma omp parallel
{
  #pragma omp single
  {
    #pragma omp task
      {printf(1);}
    #pragma omp task
      {printf(2);}
  }
}
