package uuid7

import (
	"bytes"
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

func TestGenerator(t *testing.T) {
	gen, err := NewGenerator()
	if err != nil {
		t.Fatal(err)
	}
	defer gen.Close()

	u1 := gen.Generate()
	u2 := gen.Generate()

	if (u1[6] >> 4) != 0x7 {
		t.Errorf("versi UUID1 bukan 7: %x", u1[6]>>4)
	}

	if bytes.Compare(u1[:], u2[:]) >= 0 {
		t.Error("UUID tidak monoton naik")
	}
}
