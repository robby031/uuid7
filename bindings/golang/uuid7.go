package uuid7

/*
#cgo CFLAGS: -I${SRCDIR}

#include "uuid7.h"
#include <stdlib.h>
*/
import "C"
import (
	"errors"
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

// defaultGen adalah singleton generator yang di-inisialisasi sekali saat pertama
// kali Generate() atau GenerateString() dipanggil.
var (
	defaultOnce sync.Once
	defaultGen  *Generator
)

// Generate membuat UUID v7 menggunakan generator bawaan yang di-inisialisasi otomatis.
// Aman digunakan dari banyak goroutine secara bersamaan.
func Generate() [16]byte {
	defaultOnce.Do(func() {
		var err error
		defaultGen, err = NewGenerator()
		if err != nil {
			panic("uuid7: " + err.Error())
		}
	})
	return defaultGen.Generate()
}

// GenerateString membuat UUID v7 dan langsung mengembalikan string berformat
// xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx.
func GenerateString() string {
	return UUIDToString(Generate())
}

// UUIDToString memformat UUID ke representasi string standar RFC 4122:
// xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx
// Implementasi manual tanpa fmt.Sprintf untuk meminimalkan alokasi.
func UUIDToString(uuid [16]byte) string {
	const hx = "0123456789abcdef"
	var buf [36]byte
	for i, j := 0, 0; i < 16; i++ {
		if i == 4 || i == 6 || i == 8 || i == 10 {
			buf[j] = '-'
			j++
		}
		buf[j] = hx[uuid[i]>>4]
		buf[j+1] = hx[uuid[i]&0xf]
		j += 2
	}
	return string(buf[:])
}
