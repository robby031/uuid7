# UUID v7 — WebAssembly Binding

WebAssembly binding for the [uuid7](https://github.com/robby031/uuid7) C library. Generates monotonically ordered UUID v7 values per [RFC 9562](https://www.rfc-editor.org/rfc/rfc9562).

## Build

Requires [Emscripten](https://emscripten.org/) (`emcc`).

```bash
cd bindings/wasm
make
```

This produces `uuid7.js` (Emscripten loader) and `uuid7.wasm`.

## Quick Start (Node.js)

The simplest way — call `init(module)` once, then use the module-level functions directly.

```js
const Module = require('./uuid7.js');
const uuid7  = require('./uuid7_wasm.js');

Module.onRuntimeInitialized = () => {
    uuid7.init(Module); // call once after WASM is ready

    // Generate a formatted UUID string
    console.log(uuid7.generateString());
    // → "019ee981-1e55-7ff9-ad82-63abef52f6db"

    // Or get raw bytes / hex
    console.log(uuid7.generateHex());   // no hyphens
    const bytes = uuid7.generate();     // Uint8Array(16)
};
```

## Quick Start (Browser)

```html
<script src="uuid7.js"></script>
<script src="uuid7_wasm.js"></script>
<script>
  Module.onRuntimeInitialized = () => {
    uuid7.init(Module);
    document.getElementById('out').textContent = uuid7.generateString();
  };
</script>
```

## API Reference

### Module-level functions (recommended)

Call `init(module)` once after `Module.onRuntimeInitialized` fires, then use these anywhere.

| Function | Returns | Description |
|---|---|---|
| `init(module)` | `void` | Initialize the default generator with the WASM module |
| `generate()` | `Uint8Array(16)` | Generate a UUID v7 (raw bytes) |
| `generateHex()` | `string` | Generate a UUID v7 as a 32-char hex string |
| `generateString()` | `string` | Generate and format as `xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx` |

### UUID7Generator (manual lifecycle)

Use `UUID7Generator` when you need isolated generator state or explicit resource control.

```js
const { UUID7Generator } = require('./uuid7_wasm.js');

Module.onRuntimeInitialized = () => {
    const gen = new UUID7Generator(Module);

    console.log(gen.generateString());
    // → "019ee981-1e55-7ff9-ad82-63abef52f6db"

    console.log(gen.generateHex());  // 32-char hex, no hyphens
    console.log(gen.generate());     // Uint8Array(16)

    gen.destroy(); // free WASM memory
};
```

| Method | Returns | Description |
|---|---|---|
| `new UUID7Generator(module)` | — | Create a new generator |
| `.generate()` | `Uint8Array(16)` | Generate a UUID v7 (raw bytes) |
| `.generateHex()` | `string` | Generate a UUID v7 as a 32-char hex string |
| `.generateString()` | `string` | Generate and format as `xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx` |
| `.destroy()` | `void` | Free WASM memory (idempotent) |

## Monotonic Ordering

UUID v7 values produced by the same generator are strictly monotonically increasing, even when multiple values are generated within the same millisecond. Values from different generators may interleave.
