
require "../Tools/donlp2/donlp2_wrapper.h";
require "../Tools/donlp2/donlp2_wrapper.c";
require "../Tools/donlp2/donlp2.c";
/* require "a.c"; */

extern proc donlp2_wrapper(args...) : c_int;

param bigN = 1e20;

param n = 10;
param p = 1;
param ni = n/p;
type c_vec = ni*real;
type chpl_array = [1..ni] real;
type chpl_matrix = [1..p] c_vec;


proc to_c_array(ref y: chpl_array, ref x: c_vec){
  [i in y.domain with (ref x)] x[i] = y[i];
}

proc local_prob_init(ref x : c_vec, ref low : c_vec, ref hi : c_vec, param_ : c_void_ptr){
  var y : chpl_array;
  [i in y.domain] y[i] = if i % 2 == 0 then -0.5 - i else 0;

  low = min(c_vec);
  hi = max(c_vec);
  to_c_array(y, x);
}

proc local_fun(ref x : c_vec, ref f : real, param_ : c_void_ptr){
  f = 0.5 * (+ reduce x**2);
}

proc local_con(ref x : c_vec, ref con : c_vec, param_ : c_void_ptr){
  [i in 1..ni with (ref con)] con[i] = -0.5+(-1)**i*i + x[i] - x[if i+1 > ni then 1 else i+1];
}

var xopt : chpl_matrix;
var xprev : chpl_matrix;
var fopt : c_vec;
var lamopt : chpl_matrix;

//local_prob_init(fopt, xopt[1], lamopt[1]);

 donlp2_wrapper(n, n,
  fopt,
  xopt[1],
  lamopt[1],
  c_ptrTo(local_prob_init),
  c_ptrTo(local_fun),
  c_ptrTo(local_con),
  nil
);

writeln(fopt);
writeln(xopt);
writeln(lamopt);
