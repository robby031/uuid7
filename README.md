# UUID v7

A C library for generating UUID v7 with monotonic ordering. Suitable for index databases because UUIDs are sorted by time.

## Build

```bash
cmake -B build -DBUILD_TESTS=ON -DBUILD_BENCHMARKS=ON
cmake --build build
```

## Test

```bash
cd build && ctest
```

Or use a Makefile:

```bash
make test
```

## Basic Usage

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
