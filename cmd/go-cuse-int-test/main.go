package main

import (
	"flag"
	"log"
	"os"

	"golang.org/x/sys/unix"
)

const (
	fiocGetSize = 0
	fiocSetSize = 1
)

func main() {
	dev := flag.String("dev", "/dev/go-cuse-int", "Device to open")

	flag.Parse()

	file, err := os.Open(*dev)
	if err != nil {
		panic(err)
	}

	before, err := unix.IoctlGetInt(int(file.Fd()), fiocGetSize)
	if err != nil {
		panic(err)
	}

	log.Println("Value before setting:", before)

	if err := unix.IoctlSetPointerInt(int(file.Fd()), fiocSetSize, 20); err != nil {
		panic(err)
	}

	after, err := unix.IoctlGetInt(int(file.Fd()), fiocGetSize)
	if err != nil {
		panic(err)
	}

	log.Println("Value after setting:", after)
}
