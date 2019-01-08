package main

func fun(word string) {
	print("hello ", word)
}

func main() {
	go fun("world")
}
