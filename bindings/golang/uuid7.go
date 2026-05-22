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
	"unsafe"
)

// noescape menyembunyikan pointer dari escape analysis compiler.
// Aman digunakan di sini karena uuid7_generate hanya menulis ke buffer
// selama durasi pemanggilan fungsi dan tidak menyimpan pointer setelahnya.
//
//go:nosplit
func noescape(p unsafe.Pointer) unsafe.Pointer {
	x := uintptr(p)
	return unsafe.Pointer(x ^ 0)
}

type Generator struct {
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
	C.uuid7_generate(g.ctx, (*C.uint8_t)(noescape(unsafe.Pointer(&buf[0]))))
	return buf
}

func (g *Generator) Close() {
	if g.ctx != nil {
		C.uuid7_destroy(g.ctx)
		g.ctx = nil
	}
}

func UUIDToString(uuid [16]byte) string {
	return fmt.Sprintf("%x", uuid)
}
