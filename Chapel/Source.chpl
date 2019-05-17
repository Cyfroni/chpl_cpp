
require "../Tools/donlp2/donlp2_wrapper.h";
require "../Tools/donlp2/donlp2_wrapper.c";
require "../Tools/donlp2/donlp2.c";

extern proc donlp2_wrapper(args...) : c_int;

param bigN = 1e20;
param epsilon = 1e-6;

// 1024 2048
// 32 16 8 4 2 1

config param n = 1024;
config param p = 32;
writeln(n, "#", p);
param N = n/p;
type c_vec = (N+1)*real;
type chpl_array = [1..N] real;
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
    for j in y.domain {                                // initialize Powell20
      var jabs = (i-1)*N + j;
      y[j] = if jabs % 2 == 0 then -0.5 - jabs else 0;                                            // (26)
    }
    to_c_array(y, x);
  } else {
    x = xopt[i];                                       // or get from previous iteration
  }

  low = min(c_vec);                                    // fill with minimum values
  hi = max(c_vec);                                     // fill with maximum values
}

proc local_fun(ref x : c_vec, ref f : real, ref param_ : params){
  var i = param_[1];
  var jprev = (p-2+i) % p + 1;

  f = 0.5 * (+ reduce x**2) + lamb[i]*x[N+1] - lamb[jprev]*x[2];                                // (42)
}

proc local_con(ref x : c_vec, ref con : c_vec, ref param_ : params){
  var i = param_[1];
  for l in 2..N {
     var j = (i - 1) * N + l - 1;
     con[l] = -0.5 + (-1)**j*j + x[l] - x[l+1];                                                 // (31)
  }
  if( p == 1 ) then con[N+1] = N - 0.5 + x[N+1] - x[2];                                         // (26)
}

var fopt : [1..p] real;
var lamopt : chpl_matrix;
var ft : real;

//writeln("<Chapel>\nSolving Powell20 problem for: n = ", n,', p = ', p, '...');

use Time;
var timer : Timer;
timer.start();

if (p==1) {
  var par = (1,);
  donlp2_wrapper(N, N,
    c_ptrTo(fopt[1]),
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
    forall i in 1..p{
      var par = (i,);
      donlp2_wrapper(N, N-1,
        c_ptrTo(fopt[i]),
        c_ptrTo(xopt[i]),
        c_ptrTo(lamopt[i]),
        c_ptrTo(local_prob_init),
        c_ptrTo(local_fun),
        c_ptrTo(local_con),
        c_ptrTo(par)
      );
    }

    var alfa : real = n/(k+p);
    for i in 1..p {                                                                             // (42+)
       var j = i * N;
       var c = -0.5 + (-1) ** j * j;
       var inext = i % p + 1;
       var gi = xopt[i][N + 1] - xopt[inext][2] + c;
       lamb[i] = max(0, lamb[i] + alfa * gi);
    }
    dist_x = sqrt(+ reduce (+ reduce (xopt - xprev)**2));
    xprev = xopt;
    k+=1;
  }
}


//writeln('xopt =\n', xopt);
writeln(timer.elapsed());
