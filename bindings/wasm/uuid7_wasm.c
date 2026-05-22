#include <emscripten.h>
#include "../../include/uuid7.h"
#include <stdlib.h>
#include <string.h>

void* EMSCRIPTEN_KEEPALIVE uuid7_wasm_create(void) {
    return (void*)uuid7_create();
}

void EMSCRIPTEN_KEEPALIVE uuid7_wasm_destroy(void* ctx) {
    uuid7_destroy((uuid7_ctx*)ctx);
}

void EMSCRIPTEN_KEEPALIVE uuid7_wasm_generate(void* ctx, uint8_t* out) {
    uuid7_generate((uuid7_ctx*)ctx, out);
}