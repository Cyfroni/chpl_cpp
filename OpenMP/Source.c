/******************************************************************************/
/*            powell20_ppm.c  program                                         */
/******************************************************************************/
/*
*  GENERAL DESCRIPTION
*
*  The example program that makes use of DONLP2 wrapper
*  (written by Marek Szyprowski from Warsaw University of Technology; DONLP2
*  solver itself was written by Peter Spellucci from Darmstadt University
*  of Technology) in a multicore environment. The concurrent threads
*  are created with  the help of OpenMP parallelizing directives.
*  The program solves the Powell20 problem (from the CUTE set) for the
*  dimension and the number of threads specified in the command line.
*  It implements the decomposition of the dual problem (price method,
*  Lagrange relaxation; "ppm" in the name is from "parallel price method").
*
*  THE PROBLEM SOLVED  - Powell20
*
*    min_{y \in R^n} 0.5*( y[1]^2+y[2]^2+....+y[n]^2 )
*
*       subject to
*                 y[k+1]-y[k] >= -0.5+(-1)^k*k;          k =1,...,n-1
*                  y[1]-y[n] >= n-0.5;
*
*  THE ALGORITHM (THE DECOMPOSITION IDEA)
*
*      We divide vector  y \in R^n into p equal parts  (assuming that p is a
*  divisor of an even n) and denote:
*         x[i][j]=y[(i-1)*ni+j)], i=1,...,p; j=1,...,ni,
*         x[i]=[ x[i][1], x[i][2], ... x[i][ni] ]'
*         x = [ x[1]',x[2]',..., x[p]' ]' (=y)
*  where ni=n/p.
*  Now we may divide all n constraints into p+1 groups: constraints
*  dependent only on x[i] subvector for every i=1,...,p:
*     y[k+1]-y[k] >= -0.5+(-1)^k*k,     k=(i-1)*ni+j, j=1,...,ni-1
*  that is:
*     x[i][j+1]-x[i][j] >= -0.5 +(-1)^k(i,j)*k(i,j),  j=1,...,ni-1
*  where k(i,j)= (i-1)*ni+j, defining some sets X[i] \subset R^{ni},
*  and p constraints involving x[i][j] from different subvectors - x[i]
*  and x[i+1]:
*     y[k+1]-y[k] >= -0.5+(-1)^k*k,       k=i*ni, i=1,...,p-1
*     y[1] - y[n] >= n-0.5
*  The latter constraints with the above assumptions on n and p may be
*  written as:
*     x[mod(i,p)+1][1]-x[i][ni] >= c(i), i=1,...,p
*  where c(i)=-0.5 +(-1)^(i*ni)*(i*ni)
*
*  The dual problem will be now:

*      max_{lambda >=0) min_{x[i] \in X[i], i=1,...p} { L(x,lambda) =
*         =\sum_{i=1}^p \sum_{j=1}^{ni} 0.5* x[i][j]^2 +
*         + \sum_{i=1}^p \lambda[i]*( c(i) + x[i][ni] - x[mod(i,p)+1][1]) =
*         = \sum_{i=1}^p ( \sum_{j=1}^{ni} 0.5* x[i][j]^2 +
*        +  lambda[i]*x[i][ni] - lambda[mod(p-2+i,p)+1]*x[i][1] ) +c(i)}
*
*   The inner optimization decomposes into p local problems:
*      min_{x[i] \in X[i]}   \sum_{j=1}^{ni} 0.5* x[i][j]^2 +
*             +  lambda[i]*x[i][ni] - lambda[mod(p-2+i,p)+1]*x[i][1]
*   which may be solved independently, if possible, in parallel.
*   The external - dual - problem (called coordination problem) may be
*   solved  in the simplest case by the steepest ascent gradient algorithm:
*      lambda[i] := lambda[i]+ alfa*(c(i)+xopt[i][ni]-xopt[mod(i,p)+1][1])
*                                        for i=1,2,...,p
*   where alfa is a suitably chosen step coefficient.
*
*  PARALLELIZATION  TECHNOLOGY
*
*  Actually, the whole parallelization is obtained by only one additional
*  line:
*              #pragma omp parallel for
*  Three calls of  functions from OpenMP library and the directive
*  #include <omp.h> have supplementary character.
*
*  SOURCE MODULES USED
*         donlp2_wrapper.c
*         donlp2_wrapper.h
*         donlp2_priv.h
*         donlp2_pub.h
*         donlp2.c
*
*  COMPILATION
*
*   Under SOLARIS:
*  $ cc powell20_ppm.c donlp2_wrapper.c donlp2.c -lm -xopenmp -xO3 -o powell
*
*   Under LINUX (for gcc compiler versions higher than 4.2; for some
*                distributions 4.1.1):
*  $ gcc powell20_ppm.c donlp2_wrapper.c donlp2.c -lm -fopenmp -o powell
*
*   Under MS VISUAL STUDIO 2005/2008 C/C++(Professional and Team System
*                           Editions)  in Project Properties
*     (the entrance e.g. by right-click on the Project),
*     then Properties\Configuration Properties\...
*                                     ...C/C++\Language\OpenMP Support
*     set "Yes"
*
*   RUNNING THE PROGRAM
*
*        SOLARIS, LINUX:

*           $ ./powell n p
*             where: n - dimension of the problem,
*                    p - number of local subproblems (threads)
*             e.g.
*               $ ./powell 100 4
*
*        WINDOWS 2000/XP/Vista

*         In the Command Prompt Window:
*           C:\> <name_of_executable_file> n p
*
*  AUTHOR:
*
*    Andrzej Karbowski
*  Institute of Control and Computation Engineering
*  Department of Electronics and Information Technology
*  Warsaw University of Technology
*      E-mail: A.Karbowski@ia.pw.edu.pl
*
*  DATE OF RELEASE
*
*     4 August 2007                                                           */
/******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <omp.h>

#include "../Tools/donlp2/donlp2_wrapper.h"

#define bigN 1.e20

int    p;            /*  number of local problems (threads) */
double *lambda;      /*  vector of Lagrange multipliers  for the
                         common (global) constraints  */
int    k;            /*  index of the current iteration    k=1,2,... */
double **xopt, **xprev;

struct my_param {
   int  i;   /*  identifier of the local problem i=1,...,p      */
   int  ni;  /*  dimension of the i-th local problem            */
};
/* WARNING! - all indices in arrays (vectors) are from 1 to n,
   not from 0 to n-1 ! */


/****************************************************************************/

/* subroutine that sets the initial values of variables and their boundaries */

void local_prob_init(double x[], double low[], double hi[], void *param_)
{
   struct my_param *param = (struct my_param *)param_;

   int i = param -> i;
   int ni = param -> ni;
   int j, jabs;

    /* initial value of vector x */
   if( k==1 )
      for(j=1; j<=ni; j++){
         jabs=(i-1)*ni+j;
         if( jabs%2 == 0 )
            x[j]=-0.5-jabs;
         else
            x[j]=0;
      }
   else
      for(j=1; j<=ni; j++)
         x[j]=xopt[i][j];

    /* boundaries for vector x */
   for(j=1 ; j<=ni; j++){
      /* there are no boundaries for elements of vector x in Powell20 problem,
         but setting the boundaries is required by the solver itself,
         so, as the boundaries really big positive and negative
         numbers are used */
       low[j]=-bigN;
       hi[j]=bigN;
   }
   for (int xxx = 0; xxx<=ni; xxx++){
     printf("%f ", x[xxx]);
   }
   printf("prob\n");
}

/****************************************************************************/

/* this procedure calculates the value of the i-th local objective function
   for the given vectors x and lambda */

void local_fun(double *x, double *f, void *param_)
{
   struct my_param *param = (struct my_param *)param_;
   int i = param -> i;
   int ni = param->ni;
   int j, jprev;
   double sum, aux;

   sum=0.; aux=0.;
   for(j=1; j<=ni; j++)
      sum += pow(x[j], 2);
   if( p > 1 ){
      jprev = (i-2+p) % p + 1;
      aux = lambda[i]*x[ni] - lambda[jprev]*x[1];
   }
   *f = 0.5*sum + aux;

   // for (int xxx = 0; xxx<=ni; xxx++){
   //   printf("%f ", x[xxx]);
   // }
   // printf("%f ", *f);
   // printf("fun\n");
}

/* **************************************************************************/

/* this procedure calculates the values of all constraints for
   the i-th local problem for the given vector x */

void local_con(double *x, double *con, void *param_)
{
   struct my_param *param = (struct my_param *)param_;
   int ni = param->ni;
   int i = param->i;
   int j, jabs;

   for(j=1;j<=ni-1;j++){
      jabs = (i-1)*ni + j;
      con[j] = -0.5+pow(-1,jabs)*jabs + x[j] - x[j+1];
   }
   if( p == 1 ) con[ni] = ni - 0.5 - x[1] + x[ni];

   // for (int xxx = 0; xxx<=ni; xxx++){
   //   printf("%f ", x[xxx]);
   // }
   // printf("##");
   // for (int xxx = 0; xxx<=ni; xxx++){
   //   printf("%f ", con[xxx]);
   // }
   // printf("con\n");
}

/***************************************************************************/

/* this function calculates Euclidean distance between two vectors x1 and x2
   packed into two twodimensional arrays   */

double Eucl_dist(int r, int n, double **x1, double **x2)
{
   int i, j;
   double sum;
   sum=0.;
   for(i=1;i<=r;i++)
      for(j=1;j<=n;j++) sum = sum + pow( x1[i][j]-x2[i][j], 2);
   return sqrt(sum);
}

/***************************************************************************/

int main(int argc, char **argv)
{
   int n, ni, nv, nc;
   int i, j, inext, noel;
   int kco;
   double *fopt, **lamopt, alfa;
   double dist_x, con_violQ, con_viol;
   double gi, rhc, ft, lami;
   double t0, tf;
   #define epsilon 1.e-6     /* the accuracy of approximation of the
                                optimal solution in x space */
   #define max(x,y) (  (x  >=  y)  ?  x  :  y  )

   n=atoi(argv[1]);
   p=atoi(argv[2]);

   if(argc < 3){
      printf("Syntax: %s <dimension of the problem> <number of threads> \n",
                                                                   argv[0]);
      exit(0);
   }
   if( n%p != 0 || n%2 !=0 ){
      printf(" p should be an even  divisor of n! \n");
      exit(0);
   }
   ni=n/p; k=1;
   xopt = (double**) malloc((p+1) * sizeof(double*));
   xprev = (double**) malloc((p+1) * sizeof(double*));
   fopt = (double*) malloc((p+1) * sizeof(double));
   lamopt = (double**) malloc((p+1)*sizeof(double*));
   for(i = 0; i<=p; i++){
       xopt[i] = (double*)  malloc( (ni+1) * sizeof(double));
       xprev[i] = (double*) malloc( (ni+1) * sizeof(double));
       lamopt[i] = (double*) malloc((ni+1)*sizeof(double));
       for(j=0;j<=ni;j++)
           xprev[i][j]=0.;
   }
   printf("<OpenMP>\nSolving Powell20 problem for: n=%d,  p=%d...\n", n, p);
   t0=omp_get_wtime();
   if( p == 1 ){

   /* Centralized version (that is, without decomposition)  */

      nv=n; nc=n;
      struct my_param par;
      par.i = 1;
      par.ni = n;
      donlp2_wrapper(nv, nc, &fopt[1], xopt[1], lamopt[1],
                     local_prob_init, local_fun, local_con, &par);
      ft=fopt[1];
   }
   else
   {
   /* The version with decomposition     */

      nv=ni; nc=ni-1;
      lambda = (double*) malloc((p+1)*sizeof(double));
      for(i=0; i<=p; i++)
         lambda[i]=0.;
      dist_x=1.;
      omp_set_num_threads(p);
      while( dist_x > epsilon )
      {
         #pragma omp parallel for

   /* the p independent calls of local problems  */

         for(i=1;  i <= p; i++)
         {
            struct my_param par;
            par.i = i;
            par.ni = ni;
            donlp2_wrapper(nv, nc, &fopt[i], xopt[i], lamopt[i],
                           local_prob_init, local_fun, local_con, &par);
         }
        //break;
         /* the coordination - change of Lagrange multipliers (prices for the
            violation of constraints) due to a steepest ascent gradient
            algorithm,  so as to maximize the value of the dual function ft */

         con_violQ=0.;ft=0.;
         alfa=((double) n)/(k+p);
         for(i=1;i<=p;i++){
            kco=i*ni;
            rhc=-0.5+pow(-1,kco)*kco;
            inext= i%p +1;
            gi=rhc+xopt[i][ni]-xopt[inext][1];
            con_violQ=con_violQ+pow(max(0.,gi),2);
            ft=ft+fopt[i]+lambda[i]*rhc;
            lambda[i]=max(0,lambda[i]+ alfa*gi);
         }
         dist_x=Eucl_dist(p,ni,xopt,xprev);
         con_viol=sqrt(con_violQ);
         printf(" k= %d, fun= %7.3f, cons_viol= %7.6f, dist_x= %7.6f\n",
                  k, ft, con_viol, dist_x);
         for(i=1;i<=p;i++)
            for(j=1;j<=ni;j++) xprev[i][j]=xopt[i][j];
         k=k+1;
      }
   }
   tf=omp_get_wtime();
   printf("Powell - %d:\n", n);
   printf("-----------------------------------\n");
   printf("xopt =\n");
   noel=1;
   for(i=1;i<=p;i++)
      for(j=1;j<=ni;j++){
         printf("%7.2f ",xopt[i][j]);
	      if( noel%10==0)
         printf ("\n");
         noel=noel+1;
      }
   printf("\n");
   printf("-----------------------------------\n");
   printf("fopt =%f\n",ft);
   printf("-----------------------------------\n");
   printf(" Time od calculations: %f s\n",tf-t0);
   printf("-----------------------------------\n");
   for(i=0;i<=p;i++){
      free(xopt[i]);
      free(xprev[i]);
      free(lamopt[i]);
   }
   free(xopt);
   free(xprev);
   free(fopt);
   free(lamopt);
   return 0;
}
