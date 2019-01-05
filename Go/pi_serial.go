package main

const numSteps = 1000000000
const step = 1.0 / float64(numSteps)

func main() {
	piSum := 0.0
	for i := 0; i < numSteps; i++ {
		x := (float64(i) + 0.5) * step
		piSum += 4.0 / (1.0 + x*x)
	}
	pi := piSum * step

	println(pi)
}
