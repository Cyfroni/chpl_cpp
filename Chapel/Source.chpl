
require "../Tools/donlp2/donlp2_wrapper.h";
require "../Tools/donlp2/donlp2_wrapper.c";
require "../Tools/donlp2/donlp2.c";

extern proc donlp2_wrapper(args...) : c_int;

param bigN = 1e20;
param epsilon = 1e-6;

//200 400
//20 10 5 2 1

param n = 400;
param p = 1;
param ni = n/p;
type c_vec = (ni+1)*real;
type chpl_array = [1..ni] real;
type chpl_matrix = [1..p] c_vec;
type params = 1*int;

var k = 1;
var lamb : [1..p] real;

var xopt : chpl_matrix;
var xprev : chpl_matrix;

proc to_c_array(ref y: chpl_array, ref x: c_vec){
  [j in y.domain with (ref x)] x[j+1] = y[j];
}

proc local_prob_init(ref x : c_vec, ref low : c_vec, ref hi : c_vec, ref param_ : params){
  var i = param_[1];
  var y : chpl_array;

  if (k==1){
    for j in y.domain {
    var jabs = (i-1)*ni + j;
    y[j] = if jabs % 2 == 0 then -0.5 - jabs else 0;
    }
    to_c_array(y, x);
  } else {
    x = xopt[i];
  }

  low = min(c_vec);
  hi = max(c_vec);
}

proc local_fun(ref x : c_vec, ref f : real, ref param_ : params){
  var i = param_[1];
  var sum = + reduce x**2;
  var jprev = (i-2+p) % p + 1;
  var aux = lamb[i]*x[ni+1] - lamb[jprev]*x[2];
  f = 0.5 * sum + aux;
}

proc local_con(ref x : c_vec, ref con : c_vec, ref param_ : params){
  var i = param_[1];
  for j in 2..ni {
     var jabs = (i-1)*ni + j - 1;
     con[j] = -0.5+(-1)**jabs*jabs + x[j] - x[j+1];
  }
  if( p == 1 ) then con[ni+1] = ni - 0.5 - x[2] + x[ni+1];
}

var fopt : [1..p] real;
var lamopt : chpl_matrix;
var ft : real;

writeln("<Chapel>\nSolving Powell20 problem for: n = ", n,', p = ', p, '...');

use Time;
var timer : Timer;
timer.start();

if (p==1) {
  var par = (1,);
  donlp2_wrapper(ni, ni,
    c_ptrTo(ft),
    c_ptrTo(xopt[1]),
    c_ptrTo(lamopt[1]),
    c_ptrTo(local_prob_init),
    c_ptrTo(local_fun),
    c_ptrTo(local_con),
    c_ptrTo(par)
  );
} else {
  var dist_x = 1.0;
  while dist_x > epsilon {
    for i in 1..p{
      var par = (i,);
      donlp2_wrapper(ni, ni-1,
        c_ptrTo(fopt[i]),
        c_ptrTo(xopt[i]),
        c_ptrTo(lamopt[i]),
        c_ptrTo(local_prob_init),
        c_ptrTo(local_fun),
        c_ptrTo(local_con),
        c_ptrTo(par)
      );
    }

    var con_violQ : real;
    ft = 0;
    var alfa : real = n/(k+p);
    for i in 1..p {
       var kco = i * ni;
       var rhc = -0.5 + (-1) ** kco * kco;
       var inext = i % p + 1;
       var gi = rhc + xopt[i][ni + 1] - xopt[inext][2];
       con_violQ += max(0,gi) ** 2;
       ft += fopt[i] + lamb[i] * rhc;
       lamb[i] = max(0, lamb[i] + alfa * gi);
    }
    dist_x = sqrt(+ reduce (+ reduce (xopt - xprev)**2));
    var con_viol=sqrt(con_violQ);
    //writeln(k, ' ', ft, ' ', con_viol, ' ', dist_x);
    xprev = xopt;
    k+=1;
  }
}

/* writeln('Powell - ', n);
writeln('-----------------------------------');
writeln('xopt =\n', xopt);
writeln('-----------------------------------');
writeln('fopt = ', ft); */
writeln('-----------------------------------');
writeln('Time od calculations: ', timer.elapsed(), ' s');
writeln('-----------------------------------');
