package main

import (
	"flag"
	"os"

	"github.com/pojntfx/go-cuse/pkg/cuse"
	"github.com/pojntfx/go-cuse/pkg/devices"
)

func main() {
	flag.Parse()

	device := devices.NewTraceDevice()

	if err := cuse.MountDevice(
		device,

		70,
		70,
		"go-cuse-trace",

		append([]string{os.Args[0]}, flag.Args()...),
	); err != nil {
		panic(err)
	}
}
