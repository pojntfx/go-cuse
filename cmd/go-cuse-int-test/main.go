package main

import (
	"flag"
	"log"
	"os"

	"golang.org/x/sys/unix"
)

func main() {
	dev := flag.String("dev", "/dev/go-cuse-int", "Device to open")

	flag.Parse()

	file, err := os.Open(*dev)
	if err != nil {
		panic(err)
	}

	before, err := unix.IoctlGetInt(int(file.Fd()), 1)
	if err != nil {
		panic(err)
	}

	log.Println("Value before setting:", before)

	if err := unix.IoctlSetInt(int(file.Fd()), 2, 20); err != nil {
		panic(err)
	}

	after, err := unix.IoctlGetInt(int(file.Fd()), 1)
	if err != nil {
		panic(err)
	}

	log.Println("Value after setting:", after)
}
