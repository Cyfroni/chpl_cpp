var num_steps = 1000000000;
var step = 1.0 / num_steps;
var pi_sum = 0 : real;

use Time;
var timer : Timer;
timer.start();

for i in 1..num_steps{
	var x = (i + 0.5) * step;
	pi_sum += 4 / (1 + x**2);
}
var pi = pi_sum * step;

writeln('time:\t\t', timer.elapsed());
writeln('pi:\t\t\t', pi);