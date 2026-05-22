# UUID v7

Library C untuk generate UUID v7 dengan ordering monotonic. Cocok untuk database index karena UUID diurutkan berdasarkan waktu.

## Build

```bash
cmake -B build -DBUILD_TESTS=ON -DBUILD_BENCHMARKS=ON
cmake --build build
```

## Test

```bash
cd build && ctest
```

Atau pakai Makefile:

```bash
make test
```

## Penggunaan Dasar

```c
#include "uuid7.h"

// Stack allocation (zero overhead)
uuid7_ctx ctx;
uuid7_init(&ctx);

uint8_t uuid[16];
uuid7_generate(&ctx, uuid);
```

## Bindings

- [Python](bindings/python)
- [Go](bindings/golang)
- [Rust](bindings/rust)
- [WASM](bindings/wasm)