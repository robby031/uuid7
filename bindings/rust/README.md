# UUID v7 - Rust

Binding Rust untuk library UUID v7.

## Penggunaan

```rust
use uuid7::Generator;

fn main() {
    let mut gen = Generator::new().unwrap();
    let uuid = gen.generate();
    println!("{:x?}", uuid);
}
```
