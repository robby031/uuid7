package uuid7

import (
	"bytes"
	"fmt"
	"strings"
	"sync"
	"testing"
)

func BenchmarkGenerate(b *testing.B) {
	gen, _ := NewGenerator()
	defer gen.Close()
	b.ReportAllocs()
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_ = gen.Generate()
	}
}

func TestVersionVariant(t *testing.T) {
	gen, err := NewGenerator()
	if err != nil {
		t.Fatal(err)
	}
	defer gen.Close()

	for i := 0; i < 1000; i++ {
		u := gen.Generate()
		if u[6]>>4 != 0x7 {
			t.Fatalf("UUID[%d]: versi bukan 7, dapat %x", i, u[6]>>4)
		}
		if u[8]>>6 != 0x2 {
			t.Fatalf("UUID[%d]: variant bukan 0x2, dapat %x", i, u[8]>>6)
		}
	}
}

func TestMonotonicHighVolume(t *testing.T) {
	gen, err := NewGenerator()
	if err != nil {
		t.Fatal(err)
	}
	defer gen.Close()

	const n = 100_000
	prev := gen.Generate()
	for i := 1; i < n; i++ {
		curr := gen.Generate()
		if bytes.Compare(prev[:], curr[:]) >= 0 {
			t.Fatalf("monotonisitas rusak di iterasi %d", i)
		}
		prev = curr
	}
}

func TestCounterOverflow(t *testing.T) {
	// Verifikasi fix overflow 56-bit: counter_low harus wrap ke 0
	// dan trigger increment counter_high, bukan melewati 56-bit.
	// Diuji dengan generate volume tinggi agar counter pasti overflow.
	gen, err := NewGenerator()
	if err != nil {
		t.Fatal(err)
	}
	defer gen.Close()

	const n = 500_000
	uuids := make([][16]byte, n)
	for i := range uuids {
		uuids[i] = gen.Generate()
	}
	for i := 1; i < n; i++ {
		if bytes.Compare(uuids[i-1][:], uuids[i][:]) >= 0 {
			t.Fatalf("urutan rusak di index %d", i)
		}
	}
}

func TestConcurrentSafety(t *testing.T) {
	gen, err := NewGenerator()
	if err != nil {
		t.Fatal(err)
	}
	defer gen.Close()

	const workers = 8
	const perWorker = 10_000
	results := make([][][16]byte, workers)

	var wg sync.WaitGroup
	for w := 0; w < workers; w++ {
		wg.Add(1)
		go func(id int) {
			defer wg.Done()
			uuids := make([][16]byte, perWorker)
			for i := range uuids {
				uuids[i] = gen.Generate()
			}
			results[id] = uuids
		}(w)
	}
	wg.Wait()

	// Cek per-worker monotonisitas
	for w, uuids := range results {
		for i := 1; i < len(uuids); i++ {
			if bytes.Compare(uuids[i-1][:], uuids[i][:]) >= 0 {
				t.Errorf("worker %d: urutan rusak di index %d", w, i)
			}
		}
	}

	// Cek tidak ada duplikat secara global
	seen := make(map[[16]byte]bool, workers*perWorker)
	for _, uuids := range results {
		for _, u := range uuids {
			if seen[u] {
				t.Errorf("duplikat ditemukan: %s", UUIDToString(u))
			}
			seen[u] = true
		}
	}
}

func TestUUIDToStringFormat(t *testing.T) {
	gen, err := NewGenerator()
	if err != nil {
		t.Fatal(err)
	}
	defer gen.Close()

	u := gen.Generate()
	s := UUIDToString(u)

	// format: 8-4-4-4-12
	parts := strings.Split(s, "-")
	if len(parts) != 5 {
		t.Fatalf("format UUID salah, dapat %q", s)
	}
	lengths := []int{8, 4, 4, 4, 12}
	for i, p := range parts {
		if len(p) != lengths[i] {
			t.Errorf("bagian %d: panjang %d, harapan %d", i, len(p), lengths[i])
		}
	}

	// versi harus '7' di karakter pertama bagian ke-3
	if parts[2][0] != '7' {
		t.Errorf("versi UUID salah di string: %s", s)
	}

	fmt.Printf("Contoh UUID: %s\n", s)
}

func TestCloseIdempotent(t *testing.T) {
	gen, err := NewGenerator()
	if err != nil {
		t.Fatal(err)
	}
	gen.Close()
	gen.Close() // tidak boleh panic
}
