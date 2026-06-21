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

    generateString() {
        const h = this.generateHex();
        return `${h.slice(0,8)}-${h.slice(8,12)}-${h.slice(12,16)}-${h.slice(16,20)}-${h.slice(20,32)}`;
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

// --- Convenience API: panggil init(module) sekali, lalu generate() langsung ---

let _defaultGen = null;

/**
 * Inisialisasi generator bawaan dengan WASM module.
 * Harus dipanggil sekali di dalam Module.onRuntimeInitialized.
 *
 * @param {object} module - WASM Module yang sudah siap
 */
function init(module) {
    if (!_defaultGen) {
        _defaultGen = new UUID7Generator(module);
    }
}

function _requireInit() {
    if (!_defaultGen) throw new Error("uuid7: panggil init(module) terlebih dahulu");
}

/** @returns {Uint8Array} 16 bytes UUID v7 */
function generate() {
    _requireInit();
    return _defaultGen.generate();
}

/** @returns {string} UUID v7 dalam format hex tanpa tanda hubung */
function generateHex() {
    _requireInit();
    return _defaultGen.generateHex();
}

/** @returns {string} UUID v7 dalam format xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx */
function generateString() {
    _requireInit();
    return _defaultGen.generateString();
}

if (typeof module !== 'undefined' && module.exports) {
    module.exports = { UUID7Generator, init, generate, generateHex, generateString };
}
