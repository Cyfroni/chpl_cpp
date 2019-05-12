#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <omp.h>

#include "../Tools/donlp2/donlp2_wrapper.h"

#define bigN 1.e20

int p;            /*  number of local problems*/
int threads;
double *lambda;      /*  vector of Lagrange multipliers  for the
                         common (global) constraints  */
int k;            /*  index of the current iteration    k=1,2,... */
double **xopt, **xprev;

struct my_param {
    int i;   /*  identifier of the local problem i=1,...,p      */
    int ni;  /*  dimension of the i-th local problem            */
};
/* WARNING! - all indices in arrays (vectors) are from 1 to n,
   not from 0 to n-1 ! */


/****************************************************************************/

/* subroutine that sets the initial values of variables and their boundaries */

void local_prob_init(double x[], double low[], double hi[], void *param_) {
    struct my_param *param = (struct my_param *) param_;

    int i = param->i;
    int ni = param->ni;
    int j, jabs;

    if (k == 1)
        for (j = 1; j <= ni; j++) {         // initialize Powell20
            jabs = (i - 1) * ni + j;
            if (jabs % 2 == 0)
                x[j] = -0.5 - jabs;
            else
                x[j] = 0;
        }
    else
        for (j = 1; j <= ni; j++)          // or get from previous iteration
            x[j] = xopt[i][j];

    for (j = 1; j <= ni; j++) {
        low[j] = -bigN;                    // fill with minimum values
        hi[j] = bigN;                      // fill with maximum values
    }
}

/****************************************************************************/

/* this procedure calculates the value of the i-th local objective function
   for the given vectors x and lambda */

void local_fun(double *x, double *f, void *param_) {
    struct my_param *param = (struct my_param *) param_;
    int i = param->i;
    int ni = param->ni;
    int j, jprev;
    double sum, aux;

    sum = 0.;
    aux = 0.;
    for (j = 1; j <= ni; j++)
        sum += pow(x[j], 2);
    if (p > 1) {
        jprev = (i - 2 + p) % p + 1;
        aux = lambda[i] * x[ni] - lambda[jprev] * x[1];
    }
    *f = 0.5 * sum + aux;
}

/* **************************************************************************/

/* this procedure calculates the values of all constraints for
   the i-th local problem for the given vector x */

void local_con(double *x, double *con, void *param_) {
    struct my_param *param = (struct my_param *) param_;
    int ni = param->ni;
    int i = param->i;
    int j, jabs;

    for (j = 1; j <= ni - 1; j++) {
        jabs = (i - 1) * ni + j;
        con[j] = -0.5 + pow(-1, jabs) * jabs + x[j] - x[j + 1];
    }
    if (p == 1) con[ni] = ni - 0.5 - x[1] + x[ni];
}

/***************************************************************************/

/* this function calculates Euclidean distance between two vectors x1 and x2
   packed into two twodimensional arrays   */

double Eucl_dist(int r, int n, double **x1, double **x2) {
    int i, j;
    double sum;
    sum = 0.;
    for (i = 1; i <= r; i++)
        for (j = 1; j <= n; j++) sum = sum + pow(x1[i][j] - x2[i][j], 2);
    return sqrt(sum);
}

/***************************************************************************/

int main(int argc, char **argv) {
    int n, ni, nv, nc;
    int i, j, inext, noel;
    int kco;
    double *fopt, **lamopt, alfa;
    double dist_x, con_violQ, con_viol;
    double gi, rhc, ft, lami;
    double t0, tf;
    #define epsilon 1.e-6     /* the accuracy of approximation of the
                                optimal solution in x space */
    #define max(x, y) (  (x  >=  y)  ?  x  :  y  )

    n = atoi(argv[1]);
    p = atoi(argv[2]);
    threads = atoi(argv[3]);

    if (argc < 3) {
        printf("Syntax: %s <dimension of the problem> <number of threads> \n",
               argv[0]);
        exit(0);
    }
    if (n % p != 0 || n % 2 != 0) {
        printf(" p should be an even  divisor of n! \n");
        exit(0);
    }
    ni = n / p;
    k = 1;
    xopt = (double **) malloc((p + 1) * sizeof(double *));
    xprev = (double **) malloc((p + 1) * sizeof(double *));
    fopt = (double *) malloc((p + 1) * sizeof(double));
    lamopt = (double **) malloc((p + 1) * sizeof(double *));
    for (i = 0; i <= p; i++) {
        xopt[i] = (double *) malloc((ni + 1) * sizeof(double));
        xprev[i] = (double *) malloc((ni + 1) * sizeof(double));
        lamopt[i] = (double *) malloc((ni + 1) * sizeof(double));
        for (j = 0; j <= ni; j++)
            xprev[i][j] = 0.;
    }
    //printf("<OpenMP>\nSolving Powell20 problem for: n=%d, p=%d (%d)...\n", n, p, threads);
    t0 = omp_get_wtime();
    if (p == 1) {

        /* Centralized version (that is, without decomposition)  */

        nv = n;
        nc = n;
        struct my_param par;
        par.i = 1;
        par.ni = n;
        donlp2_wrapper(nv, nc, &fopt[1], xopt[1], lamopt[1],
                       local_prob_init, local_fun, local_con, &par);
        ft = fopt[1];
    } else {
        /* The version with decomposition     */

        nv = ni;
        nc = ni - 1;
        lambda = (double *) malloc((p + 1) * sizeof(double));
        for (i = 0; i <= p; i++)
            lambda[i] = 0.;
        dist_x = 1.;
        while (dist_x > epsilon) {

            /* the p independent calls of local problems  */
            #pragma omp parallel for num_threads(threads)
            for (i = 1; i <= p; i++) {
                struct my_param par;
                par.i = i;
                par.ni = ni;
                donlp2_wrapper(nv, nc, &fopt[i], xopt[i], lamopt[i],
                               local_prob_init, local_fun, local_con, &par);
            }

            con_violQ = 0.;
            ft = 0.;
            alfa = ((double) n) / (k + p);
            for (i = 1; i <= p; i++) {
                kco = i * ni;
                rhc = -0.5 + pow(-1, kco) * kco;
                inext = i % p + 1;
                gi = rhc + xopt[i][ni] - xopt[inext][1];
                con_violQ = con_violQ + pow(max(0., gi), 2);
                ft = ft + fopt[i] + lambda[i] * rhc;
                lambda[i] = max(0, lambda[i] + alfa * gi);
            }
            dist_x = Eucl_dist(p, ni, xopt, xprev);
            con_viol = sqrt(con_violQ);

            for (i = 1; i <= p; i++)
                for (j = 1; j <= ni; j++) xprev[i][j] = xopt[i][j];
            k = k + 1;
        }
    }
    tf = omp_get_wtime();
    // printf("Powell - %d:\n", n);
    // printf("-----------------------------------\n");
    // printf("xopt =\n");
    // noel = 1;
    // for (i = 1; i <= p; i++)
    //     for (j = 1; j <= ni; j++) {
    //         printf("%7.4f ", xopt[i][j]);
    //         if (noel % 10 == 0)
    //             printf("\n");
    //         noel = noel + 1;
    //     }
    // printf("\n");
    // printf("-----------------------------------\n");
    // printf("fopt =%f\n", ft);
    printf("%f\n", tf - t0);

    for (i = 0; i <= p; i++) {
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
