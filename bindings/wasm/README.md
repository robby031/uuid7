# UUID v7 - WASM

Binding WebAssembly untuk library UUID v7.

## Build

```bash
cd bindings/wasm
make
```

## Penggunaan (Node.js)

```javascript
const { create, generate, destroy } = require('./uuid7_wasm');

const ctx = create();
const uuid = generate(ctx);
console.log(uuid);
destroy(ctx);
```

## Penggunaan (Browser)

```html
<script src="uuid7.js"></script>
<script>
  const ctx = Module._uuid7_wasm_create();
  const buf = new Uint8Array(16);
  Module._uuid7_wasm_generate(ctx, buf);
  console.log(Array.from(buf).map(b => b.toString(16).padStart(2, '0')).join(''));
  Module._uuid7_wasm_destroy(ctx);
</script>
```
