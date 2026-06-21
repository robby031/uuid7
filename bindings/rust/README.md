# UUID v7 — Rust Binding

Rust binding for the [uuid7](https://github.com/robby031/uuid7) C library. Generates monotonically ordered UUID v7 values per [RFC 9562](https://www.rfc-editor.org/rfc/rfc9562).

## Installation

Add to your `Cargo.toml`:

```toml
[dependencies]
uuid7 = { path = "bindings/rust" }
```

## Quick Start

The simplest way — call the free functions directly. No setup required.

```rust
fn main() {
    // Generate and get a formatted string in one call
    println!("{}", uuid7::generate_string());
    // → "019ee981-1e55-7ff9-ad82-63abef52f6db"

    // Or get the raw bytes
    let bytes: [u8; 16] = uuid7::generate();

    // Format any raw UUID bytes
    println!("{}", uuid7::to_string(&bytes));
}
```

The default generator is initialized lazily on first use via `OnceLock<Mutex<Generator>>` and is safe for concurrent use from multiple threads.

## API Reference

### Free functions (recommended)

| Function | Returns | Description |
|---|---|---|
| `generate()` | `[u8; 16]` | Generate a UUID v7 using the default generator |
| `generate_string()` | `String` | Generate and format as `xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx` |
| `to_string(uuid: &[u8; 16])` | `String` | Format an existing raw UUID to RFC 4122 string |

### Generator (manual lifecycle)

Use `Generator` when you need isolated state, per-thread generators, or want to avoid shared global state in tests.

```rust
use uuid7::{Generator, to_string};

fn main() {
    let mut gen = Generator::new().expect("failed to create generator");

    let u1 = gen.generate();
    let u2 = gen.generate();

    println!("{}", to_string(&u1));
    println!("{}", to_string(&u2));
} // Generator is dropped and context freed here
```

| Item | Description |
|---|---|
| `Generator::new() -> Result<Generator, String>` | Create a new generator |
| `gen.generate() -> [u8; 16]` | Generate a UUID v7 |
| `Drop` | Context is freed automatically when `Generator` goes out of scope |

## Thread Safety

- The free functions `generate()` and `generate_string()` are safe to call from multiple threads concurrently; they share a `Mutex<Generator>`.
- `Generator` itself implements `Send` but not `Sync`. Move it into a thread or wrap it in `Arc<Mutex<Generator>>` for shared concurrent access.

## Monotonic Ordering

UUID v7 values produced by the same generator are strictly monotonically increasing, even when multiple values are generated within the same millisecond. Values from different generators may interleave.
