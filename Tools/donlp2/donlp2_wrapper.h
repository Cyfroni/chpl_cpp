
/* donlp2_wrapper function - arguments:
	nv - number of variables (size of vector X)
	nc - number of constraint functions
	fopt - placeholder for the result
	xopt - placeholder for vector X - warning! - indices are from 1 to nv
	lamopt - placeholder for vector lambda (Lagrange multipliers for constraint functions)
	         - warning! - indices are from 1 to nc
	calc_init - pointer to function that fills initial values of vector X and its boundaries
	calc_fun - pointer to function that calculates the value of the optimised function
	calc_con - pointer to function that calculates that value of all constraint functions
	st - pointer to structure that is passed as a parameter to all functions
*/

#ifndef DONLP2_WRAPPER_H
#define DONLP2_WRAPPER_H

int donlp2_wrapper(int nv, int nc, double *fopt, double *xopt, double *lamopt, void (* calc_init_)(double x[], double low[], double hi[], void *state), void (* calc_func_)(double x[], double *f, void *state), void (*calc_con_)(double x[], double con[], void *state), void *state_);

#endif
