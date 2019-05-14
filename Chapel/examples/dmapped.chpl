config const m = 1000, tbSizeX = 256;
const alpha = 3.0;

const ProbDist = new dmap(new dist(GPUDist(rank=1, tbSizeX)));
const ProbSpace: domain(1) dmapped ProbDist = [1..m];
var A, B, C: [ProbSpace] real;

forall (a,b,c) in (A,B,C) do
  a = b + alpha * c;


  
