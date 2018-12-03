
require "../Tools/donlp2/donlp2_wrapper.h";
require "../Tools/donlp2/donlp2_wrapper.c";
require "../Tools/donlp2/donlp2.c";

extern proc donlp2_wrapper(args...) : c_int;

param bigN = 1e20;

param n = 10;
param p = 1;
param ni = n/p;
var dom = {1..p};
type vec = ni*real;

proc local_prob_init(ref x : vec, ref low : vec, ref hi : vec){//, param_ : c_void_ptr){
  x[4] = 5;

  low = min(vec);
  hi = max(vec);
}

proc local_fun(ref x : vec, ref f : real, param_ : c_void_ptr){
  //writeln('fun');
}

proc local_con(ref x : vec, ref con : vec, param_ : c_void_ptr){
  //writeln('con');
}

var xopt : [dom] vec;
var xprev : [dom] vec;
var fopt : vec;
var lamopt : [dom] vec;

local_prob_init(fopt, xopt[1], lamopt[1]);

/* donlp2_wrapper(n, n,
  fopt,
  xopt[1],
  lamopt[1],
  c_ptrTo(local_prob_init),
  c_ptrTo(local_fun),
  c_ptrTo(local_con),
  nil
); */

writeln(fopt);
writeln(xopt);
writeln(lamopt);
