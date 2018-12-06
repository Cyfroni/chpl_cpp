/******************************************************************************/
/*  A simple wrapper for DONLP2 solver that allows to use it in multithreaded
*  applications in OpenMP environment.
*  Author: Marek Szyprowski <marek@amiga.pl>
*  Warsaw University of Technology, Faculty of Electronics
*  and Information Technology
*
*  Version: 1.0 (26.05.2007)
*
*
*  THE ARGUMENTS
*
*     nv_          -  number of variables (size of vector x)
*     nc_          -  number of constraints
*     fopt         -  value of the objective function at the solution
*     xopt         -  solution vector;
*                          Warning! - indices are from 1 to nv
*     lambda       -  vector of Lagrange multipliers at the optimum
*                          Warning! - indices are from 1 to nc
*     calc_init_   -  pointer to function that sets initial values of
*                     vector x and its boundaries
*     calc_func_   -  pointer to objective function
*     calc_con_    -  pointer to constraints' functions
*     state_       -  pointer to structure that is passed as a parameter
*                     to the above functions
*
*
*  REMARK ON GRADIENT EXTENSION
*
*  DONLP2 wrapper in this version uses nongradient calls of donlp2 solver.
*  The modification to use the gradient information is very simple
*  and requires only few - analogous to those of ef, econ - additional
*  lines in the bodies of egradf and econgrad, change of the value of analyt
*  variable and the corresponding extension of the list of parameters
*  of donlp2_wrapper.
*
*  ACKNOWLEDGMENTS
*
*  This code has been written during the project on the "Introduction
*  to Parallel and Distributed Computing" course lectured by
*  Andrzej Karbowski.
*
* Copyright (c) 2007, Marek Szyprowski, Warsaw University of Technology
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the Warsaw University of Terchnology nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY Marek Szyprowski ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL Marek Szyprowski BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                */

/******************************************************************************/

#include <omp.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define X extern
#include "donlp2_pub.h"
#undef X

#include "donlp2_wrapper.h"

void (* calc_func)(double x[], double *f, void *state);
void (* calc_con)(double x[], double con[], void *state);
void (* calc_init)(double x[], double low[], double hi[], void *state);

void *state;
int nv, nc;

#pragma omp threadprivate (state, nv, nc, calc_func, calc_con, calc_init)

int donlp2_wrapper(int nv_, int nc_, double *fopt, double *xopt, double *lambda,
        void (* calc_init_)(double x[], double low[], double hi[], void *state),
        void (* calc_func_)(double x[], double *f, void *state),
        void (*calc_con_)(double x[], double con[], void *state), void *state_)
{
    int i, j, m;
    double *mlopt;

	nv = nv_;
	nc = nc_;

	m = 2*(nv+nc);

	state = state_;
	calc_init = calc_init_;
	calc_func = calc_func_;
	calc_con = calc_con_;

  mlopt = (double*) malloc((m+1)*sizeof(double));

	memset(mlopt, 0, (m+1)*sizeof(double));

  donlp2(fopt, xopt, mlopt);

	i = 2*nv+2;
    for(j=1; j<=nc; j++)
	{
		lambda[j] = mlopt[i];
		i+=2;
	}

	free(mlopt);

    return 0;
}


/* ************************************************************************** */
/*                              donlp2-intv size initialization               */
/* ************************************************************************** */
void user_init_size(void) {

    /* problem dimension n = dim(x), nlin=number of linear constraints
       nonlin = number of nonlinear constraints  */

    n      = nv;
    nlin   = 0;
    nonlin = nc;
    iterma = 8000;
    nstep = 20;
}

/* ************************************************************************** */
/*                              donlp2-intv standard setup                    */
/* ************************************************************************** */
void user_init(void) {
    INTEGER  i,j;

    /* name is ident of the example/user and can be set at users will       */
    /* the first static character must be alphabetic. 40 characters maximum */
    /* xst violates bounds : checks for automatic correction */
    strcpy(name,"donlp2");

    /* x is initial guess and also holds the current solution */

    /* neither the function gradients nor constraint gradiends are provided */
    analyt = FALSE;

    epsdif = 1.e-16;   /* gradients exact to machine precision */
    /* if you want numerical differentiation being done by donlp2 then:*/
    epsfcn   = 1.e-16; /* function values exact to machine precision */
    taubnd   = 0.01;
    /* bounds may be violated at most by taubnd in finite differencing */
    /*  bloc    = TRUE; */
    /* if one wants to evaluate all functions  in an independent process */
    difftype = 3;

    nreset = n;

    del0 = 1;
    tau0 = 1;
    tau  = 0.1;

    silent = TRUE; /* do not write any log to disk */

	/* setup the initial values for x and it's lower and upper constraints */
	calc_init(x, low, up, state);

    /* The constraints are linear in the demo example calculation problem
       (Powell task), but here in the wrapper are defined as non-linear
       to show more general case. */
    for(i=nv+1;i<=nv+nc;i++)
	{
        low[i]=-big;
        up[i]=0.;
    }

    return;
}

/* ************************************************************************** */
/*                               special setup                                */
/* ************************************************************************** */
void setup(void) {
    te2=TRUE;
    te3=TRUE;
    return;
}

/* ************************************************************************** */
/* the user may add additional computations using the computed solution here  */
/* ************************************************************************** */
void solchk(void) {
    return;
}


/* ************************************************************************** */
/*                             objective function                             */
/* ************************************************************************** */
void ef(DOUBLE x[],DOUBLE *fx) {
	calc_func(x, fx, state);
    return;
}

/* ************************************************************************** */
/*                        gradient of objective function                      */
/* ************************************************************************** */
void egradf(DOUBLE x[],DOUBLE gradf[]) {
    return;
}


/* ************************************************************************** */
/*  no nonlinear constraints */
/* ************************************************************************** */
void econ(INTEGER type ,INTEGER liste[], DOUBLE x[],DOUBLE con[],
              LOGICAL err[]) {
    int i, j;

    if( type == 1)
    {
		calc_con(x, con, state);
    }
    else
    {
		double con_tmp[nc+1];
		calc_con(x, con_tmp, state);

		for(i=1;i<=liste[0];i++)
        {
			j=liste[i];
			con[j] = con_tmp[j];
        }
    }

    return;
}

/* ************************************************************************** */
/* ************************************************************************** */
void econgrad(INTEGER liste[] ,INTEGER shift ,  DOUBLE x[],
               DOUBLE **grad) {
    return;
}


/* ************************************************************************** */
/*                      user functions (if bloc == TRUE)                      */
/* ************************************************************************** */
void eval_extern(INTEGER mode) {
    return;
}
