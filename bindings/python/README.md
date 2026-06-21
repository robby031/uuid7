# UUID v7 — Python Binding

Python binding for the [uuid7](https://github.com/robby031/uuid7) C library. Generates monotonically ordered UUID v7 values per [RFC 9562](https://www.rfc-editor.org/rfc/rfc9562).

## Installation

```bash
cd bindings/python
pip install .
```

Requires `cffi >= 1.0.0`.

## Quick Start

The simplest way — call the module-level functions directly. No setup or teardown required.

```python
import uuid7

# Generate and get a formatted string in one call
print(uuid7.generate_str())
# → "019ee981-1e55-7ff9-ad82-63abef52f6db"

# Or get the raw bytes
b = uuid7.generate()
print(b.hex())
# → "019ee9811e557ff9ad8263abef52f6db"
```

The default generator is initialized lazily on first use. It is safe to call from multiple threads.

## API Reference

### Module-level functions (recommended)

| Function | Returns | Description |
|---|---|---|
| `generate()` | `bytes` (16) | Generate a UUID v7 using the default generator |
| `generate_str()` | `str` | Generate and format as `xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx` |

### UUID7Generator (manual lifecycle)

Use `UUID7Generator` when you need isolated state, per-thread generators, or explicit resource control.

```python
# Heap allocation (default) — use as a context manager
with uuid7.UUID7Generator() as gen:
    b = gen.generate()
    print(b.hex())

# Stack allocation — slightly lower overhead, no heap for the context struct
gen = uuid7.UUID7Generator(heap=False)
b = gen.generate()
gen.close()
```

| Method / Parameter | Description |
|---|---|
| `UUID7Generator(heap=True)` | Create a generator. `heap=False` allocates the context on the Python stack. |
| `.generate()` → `bytes` | Generate a UUID v7 |
| `.close()` | Release resources (idempotent) |
| Context manager (`with`) | Calls `.close()` automatically on exit |

## Thread Safety

- The module-level `generate()` and `generate_str()` use a `threading.Lock` to initialize the default generator safely. Subsequent calls are lock-free at the initialization check level.
- Individual `UUID7Generator` instances are **not** thread-safe. Use one instance per thread, or wrap with your own lock.

## Monotonic Ordering

UUID v7 values produced by the same generator are strictly monotonically increasing, even when multiple values are generated within the same millisecond. Values from different generators may interleave.
