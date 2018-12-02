
#ifndef DONLP2_PUB_H
#define DONLP2_PUB_H

/* **************************************************************************** */
/*                                  o8para.h                                    */
/* **************************************************************************** */

/* Types */

typedef int         INTEGER;
typedef int         LOGICAL;
enum                {FALSE,TRUE};
typedef float       REAL;
typedef double      DOUBLE;

#include <omp.h>

/* **************************************************************************** */
/*                                  o8fuco.h                                    */
/* **************************************************************************** */

    
X LOGICAL   *val,*llow,*lup;

X INTEGER   n,nr,nres,nlin,nonlin, nstep, ndualm, mdualm;

X DOUBLE    epsmac,tolmac,deldif;

X char      name[41];

X DOUBLE    epsdif;

X LOGICAL   intakt,te0,te1,te2,te3,singul;
X LOGICAL   ident,eqres,silent,analyt,cold;

X INTEGER   icf,icgf,cfincr,*cres,*cgres;

X LOGICAL   ffuerr,*confuerr;

/*  special variables for the interface to old fashioned function */
/*  specification                                                 */
/*  can be removed is only problems in the new formulation are to be used */
X INTEGER nh,ng;

X INTEGER *nonlinlist;

X INTEGER **gunit; 

X LOGICAL *gconst; 

X LOGICAL *cfuerr; 
/* this is necessary because of the different index positions used */
/*  in the old and new versions   */

#pragma omp threadprivate (val,llow,lup,n,nr,nres,nlin,nonlin, nstep, ndualm, mdualm, \
	epsmac,tolmac,deldif,name,epsdif,intakt,te0,te1,te2,te3,singul,ident,eqres,silent,analyt,cold, \
	icf,icgf,cfincr,cres,cgres, ffuerr,confuerr, nh,ng, nonlinlist, gunit, gconst,cfuerr)

/* **************************************************************************** */
/*                                  o8comm.h                                    */
/* **************************************************************************** */

X REAL      runtim;
X REAL      optite;
X DOUBLE    **accinf;
X INTEGER   itstep,phase;

X DOUBLE    upsi,upsi0,upsi1,upsist,psi,psi0,
            psi1,psist,psimin,
            phi,phi0,phi1,phimin,fx,fx0,fx1,
            fxst,fmin_don,b2n,b2n0,xnorm,x0norm,sig0,dscal,dnorm,d0norm;
X DOUBLE    sig,sigmin,dirder,cosphi,upsim;
X DOUBLE    *x,*x0,*x1,*xmin,*d,*d0,
            *dd,*difx,*resmin;

X DOUBLE    *gradf,gfn,*qgf,*gphi0,*gphi1,
            **gres,*gresn;

X INTEGER   *perm,*perm1,*colno,rank;
X DOUBLE    **qr,*betaq,*diag,
            *cscal,
            *colle;

/* colno also used o8qpso with double length ! */

X DOUBLE    **a,scalm,scalm2,*diag0,matsc;

X INTEGER   *violis,*alist,*o8bind,
            *o8bind0, *aalist,*clist;
                        
X DOUBLE    *u,*u0,
            *w,*w1,*res,
            *res0,*res1,
            *resst,scf,scf0,
            *yu,*slack,infeas,*work;

X INTEGER   iterma;
X DOUBLE    del,del0,del01,delmin,tau0,tau,ny;
X DOUBLE    smalld,smallw,rho,rho1,eta,epsx,c1d,
            scfmax,updmy0,tauqp,taufac,taumax;

X DOUBLE    alpha,beta,theta,sigsm,sigla,delta,stptrm;
X DOUBLE    delta1,stmaxl;

X DOUBLE    level;
X INTEGER   clow,lastdw,lastup,lastch;

X FILE      *prou,*meu;

X DOUBLE    *ug,*og;

X DOUBLE    *low,*up,big;

X INTEGER   nreset;

X DOUBLE    *xst;

#pragma omp threadprivate (runtim, optite, accinf, itstep, phase, upsi,upsi0,upsi1,upsist,psi,psi0, \
	psi1,psist,psimin,phi,phi0,phi1,phimin,fx,fx0,fx1,fxst,fmin_don,b2n,b2n0,xnorm,x0norm,sig0,dscal,dnorm,d0norm, \
	sig,sigmin,dirder,cosphi,upsim, x,x0,x1,xmin,d,d0, dd,difx,resmin,gradf,gfn,qgf,gphi0,gphi1, \
	gres,gresn,perm,perm1,colno,rank,qr,betaq,diag, cscal,colle,a,scalm,scalm2,diag0,matsc, \
	violis,alist,o8bind,o8bind0,aalist,clist,u,u0,w,w1,res,res0,res1,resst,scf,scf0,yu,slack,infeas,work, \
	iterma,del,del0,del01,delmin,tau0,tau,ny,smalld,smallw,rho,rho1,eta,epsx,c1d,scfmax,updmy0,tauqp,taufac,taumax, \
	alpha,beta,theta,sigsm,sigla,delta,stptrm,delta1,stmaxl,level,clow,lastdw,lastup,lastch,prou,meu, \
	ug,og,low,up,big,nreset,xst)

/* **************************************************************************** */
/*                                   o8fint.h                                   */
/* **************************************************************************** */

X LOGICAL   bloc,valid,corr;
X INTEGER   difftype;
X DOUBLE    *xtr,*xsc,*fu,**fugrad,
            **fud,epsfcn,taubnd;

#pragma omp threadprivate (bloc,valid,corr,difftype,xtr,xsc,fu,fugrad,fud,epsfcn,taubnd)

void donlp2(double *fopt, double xopt[], double mlopt[]);

#endif
