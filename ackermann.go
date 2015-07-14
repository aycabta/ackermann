package main

import (
	"fmt"
	"flag"
	"strconv"
)

func a(m int, n int) int {
	if m == 0 {
		return n + 1;
	}
	if n == 0 {
		return a(m-1, 1);
	}
	return a(m-1, a(m, n-1));
}

func main() {
	if flag.NArg() == 2 {
		m, _ := strconv.Atoi(flag.Arg(0));
		n, _ := strconv.Atoi(flag.Arg(1));
		fmt.Printf("%d\n", a(m, n));
	}
}
