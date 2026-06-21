# UUID v7 — Go Binding

Go binding for the [uuid7](https://github.com/robby031/uuid7) C library. Generates monotonically ordered UUID v7 values per [RFC 9562](https://www.rfc-editor.org/rfc/rfc9562).

## Installation

```bash
go get github.com/robby031/uuid7/bindings/golang
```

CGO must be enabled (`CGO_ENABLED=1`, which is the default).

## Quick Start

The simplest way — call the package-level functions directly. No setup required.

```go
package main

import (
    "fmt"
    "github.com/robby031/uuid7/bindings/golang"
)

func main() {
    // Generate and get a formatted string in one call
    fmt.Println(uuid7.GenerateString())
    // → "019ee981-1e55-7ff9-ad82-63abef52f6db"

    // Or get the raw 16-byte value
    b := uuid7.Generate()
    fmt.Printf("%x\n", b)
}
```

The package-level generator is initialized lazily on first use and is safe for concurrent use from multiple goroutines.

## API Reference

### Package-level functions (recommended)

| Function | Returns | Description |
|---|---|---|
| `Generate()` | `[16]byte` | Generate a UUID v7 using the default generator |
| `GenerateString()` | `string` | Generate and format as `xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx` |
| `UUIDToString(uuid [16]byte)` | `string` | Format an existing raw UUID to RFC 4122 string |

### Generator (manual lifecycle)

Use `Generator` when you need isolated state, deterministic teardown, or want to avoid shared global state in tests.

```go
gen, err := uuid7.NewGenerator()
if err != nil {
    log.Fatal(err)
}
defer gen.Close()

u := gen.Generate()
fmt.Println(uuid7.UUIDToString(u))
```

| Method | Description |
|---|---|
| `NewGenerator() (*Generator, error)` | Create a new generator |
| `(*Generator).Generate() [16]byte` | Generate a UUID v7 |
| `(*Generator).Close()` | Release resources (idempotent) |

## Thread Safety

- The package-level `Generate()` and `GenerateString()` are safe for concurrent use from any number of goroutines.
- An individual `*Generator` is also goroutine-safe; it uses an internal `sync.Mutex` to serialize calls to the underlying C library.

## Monotonic Ordering

UUID v7 values produced by the same generator are strictly monotonically increasing, even when multiple values are generated within the same millisecond. Values from different generators in the same process may interleave.
