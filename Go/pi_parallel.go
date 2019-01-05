package main

const numSteps = 1000000000
const step = 1.0 / float64(numSteps)
const threads = 4

func partialSum(part int) (piSum float64) {
	for i := part; i < numSteps; i += threads {
		x := (float64(i) + 0.5) * step
		piSum += 4.0 / (1.0 + x*x)
	}
	return
}

func main() {
	res := make(chan float64)

	for i := 0; i < threads; i++ {
		go func(i int) {
			res <- partialSum(i)
		}(i)
	}

	pi := 0.0
	for i := 0; i < threads; i++ {
		pi += <-res
	}
	pi *= step

	println(pi)
}
