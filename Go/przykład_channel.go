package main

func fun(c chan int, val int) {
	c <- val
}

func main() {
	c := make(chan int)

	for i := 0; i < 8; i++ {
		go fun(c, i)
	}

	for i := 0; i < 8; i++ {
		println(<-c)
	}
}
