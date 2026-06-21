# Changelog

## v1.1.0
- **New**: Package-level / module-level convenience functions across all bindings — no generator setup required
  - Go: `Generate() [16]byte`, `GenerateString() string`
  - Python: `generate() -> bytes`, `generate_str() -> str`
  - Rust: `generate() -> [u8; 16]`, `generate_string() -> String`, `to_string(uuid: &[u8; 16]) -> String`
  - WASM/JS: `init(module)`, `generate()`, `generateHex()`, `generateString()`; `generateString()` added to `UUID7Generator` class
- **New**: All binding READMEs rewritten in English with full API reference tables and thread-safety notes

## v1.0.5
- **Fix critical**: counter_low overflow 56-bit tidak terdeteksi — `++counter_low == 0` tidak pernah terpenuhi karena nilai dimulai dari hasil mask 56-bit, menyebabkan monotonisitas rusak setelah overflow. Fix: mask setelah increment `(counter_low + 1) & 0x00FFFFFFFFFFFFFF`
- **Fix**: `uuid7_generate_ex` tidak ada null guard untuk `ctx` dan `out` — bisa crash jika dipanggil langsung
- **Fix**: Rust `unsafe impl Sync` dihapus — type dengan C mutable state tidak aman di-share via `&T` antar thread
- **Fix**: `UUIDToString` di Go sekarang menghasilkan format standar RFC 4122 dengan dash (`xxxxxxxx-xxxx-7xxx-xxxx-xxxxxxxxxxxx`)
- **Fix**: Windows build — `prng.c` include `windows.h` sebelum `bcrypt.h` dan hapus `WIN32_LEAN_AND_MEAN`
- Test Go diperluas: concurrent safety, volume monotonicity, counter overflow, format string, close idempotent
- Sinkronisasi versi `CMakeLists.txt` ke 1.0.5

## v1.0.1
- Fix distribution: bundle C source files into Go, Python, and Rust bindings so they compile standalone without the full repository

## v1.0.0
- Initial release
- RFC 9562 UUID v7 support
- Monotonic timestamp generator
- Thread-safe random generator
- CMake support
- Linux/macOS/Windows build support