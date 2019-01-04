package main

import (
	"math"
	"sync"
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

	res := make([]float64, threads)

	var wg sync.WaitGroup
	for i := range res {
		wg.Add(1)
		go func(i int) {
			res[i] = partial_sum(i)
			wg.Done()
		}(i)
	}
	wg.Wait()

	pi := 0.0
	for _, result := range res {
		pi += result
	}
	pi *= step

	println("threads:\t", threads)
	println("time:\t\t", time.Since(start).Seconds())
	println("pi:\t\t\t", pi)
}
