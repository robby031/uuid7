package uuid7

/*
#cgo CFLAGS: -I${SRCDIR}

#include "uuid7.h"
#include <stdlib.h>
*/
import "C"
import (
	"errors"
	"fmt"
	"sync"
	"unsafe"
)

//go:nosplit
func noescape(p unsafe.Pointer) unsafe.Pointer {
	x := uintptr(p)
	return unsafe.Pointer(x ^ 0)
}

type Generator struct {
	mu  sync.Mutex
	ctx *C.uuid7_ctx
}

func NewGenerator() (*Generator, error) {
	ctx := C.uuid7_create()
	if ctx == nil {
		return nil, errors.New("uuid7: gagal membuat konteks")
	}
	return &Generator{ctx: ctx}, nil
}

func (g *Generator) Generate() [16]byte {
	var buf [16]byte
	g.mu.Lock()
	C.uuid7_generate(g.ctx, (*C.uint8_t)(noescape(unsafe.Pointer(&buf[0]))))
	g.mu.Unlock()
	return buf
}

func (g *Generator) Close() {
	g.mu.Lock()
	defer g.mu.Unlock()
	if g.ctx != nil {
		C.uuid7_destroy(g.ctx)
		g.ctx = nil
	}
}

// UUIDToString memformat UUID ke representasi string standar RFC 4122:
// xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx
func UUIDToString(uuid [16]byte) string {
	return fmt.Sprintf("%x-%x-%x-%x-%x",
		uuid[0:4],
		uuid[4:6],
		uuid[6:8],
		uuid[8:10],
		uuid[10:16],
	)
}
