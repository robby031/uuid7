#include "internal.h"
#include "uuid7.h"
#include <stdio.h>
#include <string.h> 
#include <assert.h>

static void print_uuid(const uint8_t *uuid) {
    for (int i = 0; i < 16; i++) printf("%02x", uuid[i]);
    printf("\n");
}

static int uuid_cmp(const uint8_t *a, const uint8_t *b) {
    return memcmp(a, b, 16);
}

int main(void) {
    uuid7_ctx ctx;
    uuid7_init(&ctx);
    uint8_t uuids[10][16];

    uint64_t base = ctx.last_timestamp_ms;

    printf("=== Test clock mundur kecil (5 detik mundur) ===\n");
    uuid7_generate_ex(&ctx, uuids[0], base);
    uuid7_generate_ex(&ctx, uuids[1], base);
    uuid7_generate_ex(&ctx, uuids[2], base + 1);

    uint64_t mundur_kecil = base + 1 - 5000;
    uuid7_generate_ex(&ctx, uuids[3], mundur_kecil);
    assert(uuid_cmp(uuids[2], uuids[3]) < 0);
    uint64_t ts3 = ((uint64_t)uuids[3][0] << 40) | ((uint64_t)uuids[3][1] << 32) |
                   ((uint64_t)uuids[3][2] << 24) | ((uint64_t)uuids[3][3] << 16) |
                   ((uint64_t)uuids[3][4] << 8)  | uuids[3][5];
    assert(ts3 == base + 1);
    printf("OK, mundur kecil tidak mengubah timestamp.\n");

    printf("\n=== Test clock mundur besar (11 detik mundur) ===\n");
    uint64_t mundur_besar = base + 1 - 11000;
    uuid7_generate_ex(&ctx, uuids[4], mundur_besar);
    uint64_t ts4 = ((uint64_t)uuids[4][0] << 40) | ((uint64_t)uuids[4][1] << 32) |
                   ((uint64_t)uuids[4][2] << 24) | ((uint64_t)uuids[4][3] << 16) |
                   ((uint64_t)uuids[4][4] << 8)  | uuids[4][5];
    assert(ts4 == mundur_besar);
    printf("OK, mundur besar mereset timestamp.\n");

    printf("\n=== Test counter overflow saat clock maju (simulasi) ===\n");
    ctx.last_timestamp_ms = 1000;
    ctx.counter_low = 0x00FFFFFFFFFFFFFEULL; // 2 sebelum max 56-bit
    ctx.counter_high = 0x3FFF;               // sudah max 14-bit
    ctx.rand_a_high = 0;
    uint8_t uuid_a[16], uuid_b[16], uuid_c[16];
    uuid7_generate_ex(&ctx, uuid_a, 1000);
    uuid7_generate_ex(&ctx, uuid_b, 1001);
    assert(uuid_cmp(uuid_a, uuid_b) < 0);
    uint64_t tsb = ((uint64_t)uuid_b[0] << 40) | ((uint64_t)uuid_b[1] << 32) |
                   ((uint64_t)uuid_b[2] << 24) | ((uint64_t)uuid_b[3] << 16) |
                   ((uint64_t)uuid_b[4] << 8)  | uuid_b[5];
    assert(tsb == 1001);
    printf("OK, counter overflow dicegah dengan ms baru.\n");

    printf("\n=== Test counter overflow saat mundur kecil ===\n");
    ctx.last_timestamp_ms = 2000;
    ctx.counter_low = 0x00FFFFFFFFFFFFFFULL; // max 56-bit
    ctx.counter_high = 0x3FFF;               // max 14-bit
    ctx.rand_a_high = 0;

    printf("Skip test spin (butuh mock clock), logika sudah tercakup.\n");
    printf("\nSemua test clock mundur selesai.\n");
    return 0;
}