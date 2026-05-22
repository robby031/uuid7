#include "uuid7.h"
#include <stdio.h>
#include <inttypes.h>

int main(void) {
    /* ==========================================
     * Contoh 1: Stack allocation (zero heap)
     * ========================================== */
    printf("=== Stack Allocation ===\n");
    uuid7_ctx ctx;
    uuid7_init(&ctx);

    uint8_t uuid1[16], uuid2[16];
    uuid7_generate(&ctx, uuid1);
    uuid7_generate(&ctx, uuid2);

    /* Cetak sebagai hex string (RFC 9562 format) */
    printf("UUID 1: ");
    for (int i = 0; i < 16; i++) {
        printf("%02x", uuid1[i]);
        if (i == 3 || i == 5 || i == 7 || i == 9) printf("-");
    }
    printf("\n");

    printf("UUID 2: ");
    for (int i = 0; i < 16; i++) {
        printf("%02x", uuid2[i]);
        if (i == 3 || i == 5 || i == 7 || i == 9) printf("-");
    }
    printf("\n");

    /* Verifikasi versi UUID 7 */
    uint8_t version = uuid1[6] >> 4;
    printf("Versi: %d\n", version);

    /* Verifikasi urutan (monoton) */
    int cmp = 0;
    for (int i = 0; i < 16; i++) {
        if (uuid1[i] != uuid2[i]) {
            cmp = (uuid1[i] < uuid2[i]) ? -1 : 1;
            break;
        }
    }
    printf("Urutan: %s\n\n", cmp < 0 ? "UUID1 < UUID2 (monoton naik)" : "TIDAK MONOTON");

    /* ==========================================
     * Contoh 2: Heap allocation
     * ========================================== */
    printf("=== Heap Allocation ===\n");
    uuid7_ctx *heap_ctx = uuid7_create();
    if (!heap_ctx) {
        fprintf(stderr, "Gagal membuat konteks heap\n");
        return 1;
    }

    uint8_t uuid3[16];
    uuid7_generate(heap_ctx, uuid3);

    printf("UUID 3: ");
    for (int i = 0; i < 16; i++) {
        printf("%02x", uuid3[i]);
        if (i == 3 || i == 5 || i == 7 || i == 9) printf("-");
    }
    printf("\n");

    uuid7_destroy(heap_ctx);
    return 0;
}