var num_steps = 1000000000;
var step = 1.0 / num_steps;
var pi_sum = 0 : real;

forall i in 1..num_steps with (+ reduce pi_sum){
	var x = (i + 0.5) * step;
	pi_sum += 4 / (1 + x**2);
}
var pi = pi_sum * step;

writeln(pi);