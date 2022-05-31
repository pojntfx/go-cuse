package devices

import (
	"log"
	"unsafe"

	"github.com/pojntfx/go-cuse/pkg/cuse"
)

type IntDevice struct {
	value int
}

func NewIntDevice() *IntDevice {
	return &IntDevice{}
}

func (d *IntDevice) Init(userdata cuse.Void, conn cuse.Conn) {}

func (d *IntDevice) InitDone(userdata cuse.Void) {}

func (d *IntDevice) Destroy(userdata cuse.Void) {}

func (d *IntDevice) Open(req cuse.Request, fi cuse.FileInfo) {
	if err := cuse.ReplyOpen(req, fi); err != nil {
		panic(err)
	}
}

func (d *IntDevice) Read(req cuse.Request, size cuse.Size, off cuse.Offset, fi cuse.FileInfo) {
	if err := cuse.ReplyBuf(req, []byte{}); err != nil {
		panic(err)
	}
}

func (d *IntDevice) Write(req cuse.Request, buf cuse.Buffer, size cuse.Size, off cuse.Offset, fi cuse.FileInfo) {
	if err := cuse.ReplyWrite(req, int(cuse.SizeToUint64(size))); err != nil {
		panic(err)
	}
}

func (d *IntDevice) Flush(req cuse.Request, fi cuse.FileInfo) {
	if err := cuse.ReplyError(req, 0); err != nil {
		panic(err)
	}
}

func (d *IntDevice) Release(req cuse.Request, fi cuse.FileInfo) {
	if err := cuse.ReplyError(req, 0); err != nil {
		panic(err)
	}
}

func (d *IntDevice) Fsync(req cuse.Request, datasync int, fi cuse.FileInfo) {
	if err := cuse.ReplyError(req, 0); err != nil {
		panic(err)
	}
}

func (d *IntDevice) Ioctl(req cuse.Request, cmd int, arg cuse.Void, fi cuse.FileInfo, flags uint, inputBuf cuse.Void, inputBufSize cuse.Size, outBufSize cuse.Size) {
	log.Println("Ioctl", req, cmd, arg, fi, flags, inputBuf, inputBufSize, outBufSize)

	switch cmd {
	case 1:
		if err := cuse.ReplyIoctl(req, 0, unsafe.Pointer(&d.value), cuse.Size(unsafe.Sizeof(d.value))); err != nil {
			panic(err)
		}
	default:
		panic("unknown command")
	}

}

func (d *IntDevice) Poll(req cuse.Request, fi cuse.FileInfo, ph cuse.PollHandle) {
	if err := cuse.ReplyPoll(req, 0); err != nil {
		panic(err)
	}
}
