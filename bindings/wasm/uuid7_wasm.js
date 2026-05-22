class UUID7Generator {
    constructor(module) {
        this._mod = module;
        this._ctx = module._uuid7_wasm_create();
        if (!this._ctx) {
            throw new Error("Gagal membuat konteks UUID7");
        }
        this._bufPtr = module._malloc(16);
    }

    generate() {
        this._mod._uuid7_wasm_generate(this._ctx, this._bufPtr);
        return new Uint8Array(this._mod.HEAPU8.buffer, this._bufPtr, 16).slice();
    }

    generateHex() {
        const bytes = this.generate();
        return Array.from(bytes).map(b => b.toString(16).padStart(2, '0')).join('');
    }

    destroy() {
        if (this._bufPtr) {
            this._mod._free(this._bufPtr);
            this._bufPtr = null;
        }
        if (this._ctx) {
            this._mod._uuid7_wasm_destroy(this._ctx);
            this._ctx = null;
        }
    }
}

if (typeof module !== 'undefined' && module.exports) {
    module.exports = { UUID7Generator };
}