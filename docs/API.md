# UUID v7 Library API

A C library for generating monotonically increasing v7 UUIDs, database-friendly, super-fast, and memory-leak-free.

## Key Features

- **RFC 9562-compliant UUID v7**: 48-bit Unix epoch timestamp, version 0x7, variant 10xx.
- **Strictly increasing monotonically**: 70-bit per millisecond counter guarantees byte order is always increasing.
- **Scalar match precision**: `memcmp(uuid1, uuid2) < 0` is equivalent to chronological order.
- **Zero overhead stack allocation**: no `malloc`/`free`, ideal for embedded/real-time.
- **Thread-safe with separate contexts**: no global state.
- **Cross-platform**: Linux, macOS, Windows, and other platforms via fallback.
- **Performance of 50+ million UUIDs/second** on modern machines.

## Data Type

### `uuid7_ctx`
UUID generator context. Allocated transparently (stack/heap).

### `uuid7_prng`
State PRNG xoshiro256** (4x64-bit), used internally.

## Heap API

### `uuid7_ctx* uuid7_create(void)`
Allocates and initializes a new context on the heap.
**Return**: Pointer to the context, or `NULL` if failed.

### `void uuid7_destroy(uuid7_ctx *ctx)`
Destroys the context created by `uuid7_create()`.

## Stack API

### `void uuid7_init(uuid7_ctx *ctx)`
Initializes an existing context in memory (stack/global/struct member).
Does not perform heap allocation.

## Generation Functions

### `void uuid7_generate(uuid7_ctx *ctx, uint8_t out[16])`
Generates a v7 UUID and writes it to a 16-byte buffer.
The generated UUID is strictly monotonically increasing within a single context.

## Usage Example

### Stack Allocation (Recommended)

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

### Heap Allocation (For Binding/FFI)

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

### RFC 9562 Compliance
- **Timestamp**: 48-bit big-endian, Unix epoch milliseconds.
- **Version**: 0x7 in the upper nibble of the 6th byte.
- **Variant**: 0b10xx in the upper 2 bits of the 8th byte.
- **Random bits**: 70-bit counter + 4-bit rand_a per ms.

### Performance Notes
- **Throughput**: ~50 million UUIDs/s (stack, modern Intel/Apple Silicon).
- **Latency**: ~20 ns per UUID.
- **Hot path (same ms)**: only 70-bit integer increment and serialization. No syscalls when milliseconds are equal.

### Database Integration
Because UUIDs always increase bytewise, a B-Tree index on a UUID column will always append to the end, minimizing page splits and fragmentation.

### Build & Installation

```bash
cmake -B build
cmake --build build
cmake --install build
```
