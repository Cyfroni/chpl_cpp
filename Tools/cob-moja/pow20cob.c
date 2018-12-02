
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "cobyla.h"

cobyla_function calcfc;

typedef struct 
{             /* To jest struktura (definiowana przez uzytkownika
                 do przekazywania do funkcji calcfc liczacej wartosc 
                 wskaznika oraz ograniczen bez uzywania zmiennych globalnych  */
  int parI;
  float parF;  // 
} example_state;
/*-------------------------------------------------------------------------*/
void Pow20_init(double *x, int n)
{
   int i;
   /* initial value of vector x */
   for(i=1; i<=n; i++){ 
         if( i%2 == 0 )
            x[i]=-0.5-i;
         else
            x[i]=0;
      }
}
/*-------------------------------------------------------------------------*/

int calcfc(int n, int m, double *x, double *f, double *con, void *state_)
{
   example_state *state = (example_state *)state_;
   int i, j;
   double sum;

   sum=0.; 
   for(i=1; i<=n; i++) 
      sum += pow(x[i], 2);
   *f = 0.5*sum;  
   for(j=1;j<=m-1; j++) con[j]= -0.5+pow(-1,j)*j + x[j]-x[j+1];
   con[m]=n-0.5+x[n]-x[1];
   return 0;
} /* calcfc */

/*-------------------------------------------------------------------------*/
int main(int argc, char **argv)
{

   double *x, *con, ft;
   int n, m;
   int i, noel;
   int errcode, iprint, maxfun;
   double rhobeg, rhoend;
   example_state state;
   
   n=atoi(argv[1]);
   
   if(argc < 2){
      printf("Syntax: %s <dimension of the problem> \n",
                                                                   argv[0]);
      exit(0);
   }
   m=n;
   x = (double*) malloc((n+1) * sizeof(double));  
   con = (double*) malloc((m+1)*sizeof(double));
  
   Pow20_init(x,n);
   printf("Solving Powell20 problem for: n=%d ...\n", n); 
   rhobeg = .5;   // parametr sympleksu poczatkowego, moze byc tyle
   rhoend = 1e-6; // jak wyzej
   iprint=1;      // wydruk tylko na koncu
   maxfun=3500;  // maksymalna liczba wywolan calcfc
   errcode= cobyla(n, m, &x[0], rhobeg, rhoend, iprint, &maxfun, calcfc, &state);
   if(errcode) printf("Blad wykonania: %d\n",errcode);
   
   printf("\n Moje wydruki\n");
   noel=1;
   for(i=1;i<=n;i++){ 
      printf("%7.2f ",x[i]);
      if( noel%10==0) 
         printf ("\n");
      noel=noel+1;
   }
   calcfc(n,  m, x, &ft, con, &state);
   printf("\n");
   printf("-----------------------------------\n");
   printf("fopt =%f\n",ft);
   printf("-----------------------------------\n");
   
   free(x);
   free(con);
   return 0;
}


