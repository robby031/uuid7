#include "internal.h"
#include "uuid7.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static int uuid_cmp(const uint8_t *a, const uint8_t *b) {
    return memcmp(a, b, 16);
}

static uint64_t extract_timestamp(const uint8_t *uuid) {
    return ((uint64_t)uuid[0] << 40) | ((uint64_t)uuid[1] << 32) |
           ((uint64_t)uuid[2] << 24) | ((uint64_t)uuid[3] << 16) |
           ((uint64_t)uuid[4] << 8)  | uuid[5];
}

static void test_counter_overflow_controlled(void) {
    uuid7_ctx ctx;
    uuid7_init(&ctx);

    ctx.last_timestamp_ms = 1000;
    ctx.counter_low  = 0x00FFFFFFFFFFFFFFULL;  // max 56-bit
    ctx.counter_high = 0x3FFF;                 // max 14-bit
    ctx.rand_a_high  = 0x5;

    uint8_t u[4][16];

    uuid7_generate_ex(&ctx, u[0], 1001);

    uint64_t ts0 = extract_timestamp(u[0]);
    assert(ts0 == 1001);
    printf("UUID[0] ts=1001 ok\n");

    uuid7_generate_ex(&ctx, u[1], 1001);
    uuid7_generate_ex(&ctx, u[2], 1001);
    assert(uuid_cmp(u[1], u[2]) < 0);

    printf("Monotonic dalam ms sama setelah overflow ok\n");

    assert(extract_timestamp(u[1]) == 1001);
    assert(extract_timestamp(u[2]) == 1001);
}

static void test_counter_56bit_overflow(void) {
    uuid7_ctx ctx;
    uuid7_init(&ctx);

    ctx.last_timestamp_ms = 2000;
    ctx.counter_low = 0x00FFFFFFFFFFFFFFFEULL; // 2 sebelum max
    ctx.counter_high = 0;
    ctx.rand_a_high = 0xA;

    uint8_t a[16], b[16], c[16];

    uuid7_generate_ex(&ctx, a, 2000);
    uuid7_generate_ex(&ctx, b, 2000);
    uuid7_generate_ex(&ctx, c, 2000);

    // Pastikan urutan naik
    assert(uuid_cmp(a, b) < 0);
    assert(uuid_cmp(b, c) < 0);

    // Periksa timestamp tetap 2000
    assert(extract_timestamp(a) == 2000);
    assert(extract_timestamp(b) == 2000);
    assert(extract_timestamp(c) == 2000);

    uint8_t expected_byte7 = (1 >> 6) & 0xFF; // 0
    uint8_t expected_byte8_low = 1 & 0x3F;    // 1
    assert(b[7] == expected_byte7);
    assert((b[8] & 0x3F) == expected_byte8_low);

    printf("Overflow 56-bit counter sukses.\n");
}

#define STRESS_COUNT 5000000
static void test_stress_volume(void) {
    uuid7_ctx ctx;
    uuid7_init(&ctx);

    printf("Menghasilkan %d UUID...\n", STRESS_COUNT);
    uint8_t prev[16];
    uint8_t curr[16];

    uuid7_generate(&ctx, prev);
    for (int i = 1; i < STRESS_COUNT; i++) {
        uuid7_generate(&ctx, curr);
        if (uuid_cmp(prev, curr) >= 0) {
            printf("ERROR: urutan tidak naik di iterasi %d\n", i);
            assert(0);
        }
        memcpy(prev, curr, 16);
    }
    printf("\n%d UUID berhasil, semua monoton naik.\n", STRESS_COUNT);
}

int main(void) {
    printf("\nUUID v7 Stress & Overflow Tests\n");
    printf("================================\n");

    printf("\n1. Simulasi overflow 56-bit counter:\n");
    test_counter_56bit_overflow();

    printf("\n2. Overflow penuh 70-bit dengan reset timestamp:\n");
    test_counter_overflow_controlled();

    printf("\n3. Stress test %d UUID:\n", STRESS_COUNT);
    test_stress_volume();

    return 0;
}