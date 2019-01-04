package main

import (
	"math"
	"time"
)

const num_steps = 1000000000
const step = 1.0 / float64(num_steps)
const threads = 4

func partial_sum(part int) (pi_sum float64) {
	for i := part; i < num_steps; i += threads {
		x := (float64(i) + 0.5) * step
		pi_sum += 4.0 / (1.0 + math.Pow(x, 2.0))
	}
	return
}

func main() {
	start := time.Now()

	res := make(chan float64)

	for i := 0; i < threads; i++ {
		go func(i int) {
			res <- partial_sum(i)
		}(i)
	}

	pi := 0.0
	for i := 0; i < threads; i++ {
		pi += <-res
	}
	pi *= step

	println("threads:\t", threads)
	println("time:\t\t", time.Since(start).Seconds())
	println("pi:\t\t\t", pi)
}
