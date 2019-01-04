package main

import (
	"math"
	"time"
)

const num_steps = 1000000000
const step = 1.0 / float64(num_steps)

func main() {
	start := time.Now()

	pi_sum := 0.0
	for i := 0; i < num_steps; i++ {
		x := (float64(i) + 0.5) * step
		pi_sum += 4.0 / (1.0 + math.Pow(x, 2.0))
	}
	pi := pi_sum * step

	println("time:\t\t", time.Since(start).Seconds())
	println("pi:\t\t\t", pi)
}
