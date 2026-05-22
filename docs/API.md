# UUID v7 Library API

Library C untuk menghasilkan UUID v7 yang monoton naik, ramah index database, super cepat, bebas memory leak.

## Fitur Utama

- **UUID v7 sesuai RFC 9562**: timestamp 48-bit Unix epoch, versi 0x7, variant 10xx.
- **Monoton naik ketat**: 70-bit counter per milidetik menjamin urutan byte selalu naik.
- **Scalar match presisi**: `memcmp(uuid1, uuid2) < 0` setara dengan urutan waktu.
- **Zero overhead stack allocation**: tanpa `malloc`/`free`, ideal untuk embedded/real-time.
- **Thread-safe dengan konteks terpisah**: tidak ada global state.
- **Cross-platform**: Linux, macOS, Windows, dan platform lain via fallback.
- **Performa 50+ juta UUID/detik** pada mesin modern.

## Tipe Data

### `uuid7_ctx`
Konteks generator UUID. Dialokasikan secara transparan (stack/heap).

### `uuid7_prng`
State PRNG xoshiro256** (4×64-bit), digunakan internal.

## API Heap

### `uuid7_ctx* uuid7_create(void)`
Alokasi dan inisialisasi konteks baru di heap.  
**Return**: Pointer ke konteks, atau `NULL` jika gagal.

### `void uuid7_destroy(uuid7_ctx *ctx)`
Hancurkan konteks yang dibuat `uuid7_create()`.

## API Stack

### `void uuid7_init(uuid7_ctx *ctx)`
Inisialisasi konteks yang sudah ada di memori (stack/global/anggota struct).  
Tidak melakukan alokasi heap.

## Fungsi Generasi

### `void uuid7_generate(uuid7_ctx *ctx, uint8_t out[16])`
Bangkitkan UUID v7 dan tulis ke buffer 16-byte.  
UUID yang dihasilkan monoton naik secara ketat dalam satu konteks.

## Contoh Penggunaan

### Stack Allocation (Disarankan)

```c
#include "uuid7.h"
#include <stdio.h>

int main() {
    uuid7_ctx ctx;
    uuid7_init(&ctx);

    uint8_t uuid[16];
    uuid7_generate(&ctx, uuid);

    for (int i = 0; i < 16; i++) printf("%02x", uuid[i]);
    printf("\n");
    return 0;
}
```

### Heap Allocation (Untuk Binding/FFI)

```c
#include "uuid7.h"
#include <stdio.h>

int main() {
    uuid7_ctx *ctx = uuid7_create();
    if (!ctx) return 1;

    uint8_t uuid[16];
    uuid7_generate(ctx, uuid);

    uuid7_destroy(ctx);
    return 0;
}
```

### Kepatuhan RFC 9562
- **Timestamp**: 48-bit big-endian, milidetik Unix epoch.
- **Versi**: 0x7 pada nibble atas byte ke-6.
- **Variant**: 0b10xx pada 2 bit atas byte ke-8.
- **Random bits**: 70-bit counter + 4-bit rand_a per ms.

### Catatan Performa
- **Throughput**: ~50 juta UUID/detik (stack, Intel/Apple Silicon modern).
- **Latensi**: ~20 ns per UUID.
- **Hot path (ms sama)**: hanya increment 70-bit integer dan serialisasi. Tidak ada syscall saat milidetik sama.

### Integrasi Database
Karena UUID selalu naik secara bytewise, index B-Tree pada kolom UUID akan selalu melakukan append di ujung, meminimalkan page split dan fragmentasi.

### Build & Instalasi

```bash
cmake -B build
cmake --build build
cmake --install build
```