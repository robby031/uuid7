#include "internal.h" // uuid7_generate_ex
#include "uuid7.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

/* ============================================================
 * RFC 9562 UUID v7 Layout
 * ============================================================
 * Byte  0-5 : timestamp_ms (48-bit big-endian, Unix epoch)
 * Byte  6   : versi 0x7 di nibble atas, rand_a di nibble bawah
 * Byte  7   : rand_b[0] (8-bit)
 * Byte  8   : variant 0b10xx di 2 bit atas, rand_b[1] di 6 bit rendah
 * Byte  9-15: rand_b[2..7] (56-bit)
 *
 * Di library ini, layout internal PRNG dan counter disesuaikan untuk memudahkan
 * implementasi:
 * - rand_a 4-bit diambil dari rand_a_high, disimpan di byte 6 rendah
 * - 70-bit counter disebar:
 *   * 8 bit tinggi dari counter_high di byte 7
 *   * 6 bit rendah dari counter_high di byte 8 rendah
 *   * 56 bit counter_low di byte 9-15
 */

static uint64_t get_timestamp(const uint8_t *uuid) {
  return ((uint64_t)uuid[0] << 40) | ((uint64_t)uuid[1] << 32) |
         ((uint64_t)uuid[2] << 24) | ((uint64_t)uuid[3] << 16) |
         ((uint64_t)uuid[4] << 8) | uuid[5];
}

static int get_version(const uint8_t *uuid) { return uuid[6] >> 4; }

static int get_variant(const uint8_t *uuid) { return uuid[8] >> 6; }

static void test_version_variant_consistency(void) {
  uuid7_ctx ctx;
  uuid7_init(&ctx);
  uint8_t u[5][16];

  uuid7_generate(&ctx, u[0]); // normal
  uuid7_generate(&ctx, u[1]); // ms sama
  uint64_t start = ctx.last_timestamp_ms;
  for (int limit = 0; ctx.last_timestamp_ms == start && limit < 100000;
       limit++) {
    uuid7_generate(&ctx, u[0]); // overwrite until ms change
  }
  uuid7_generate(&ctx, u[2]);               // ms baru
  uint64_t now = ctx.last_timestamp_ms - 5; // mundur 5 ms
  uuid7_generate_ex(&ctx, u[3], now);
  ctx.counter_low = 0x00FFFFFFFFFFFFFFULL;
  ctx.counter_high = 0x3FFF;
  uuid7_generate_ex(&ctx, u[4], ctx.last_timestamp_ms + 1);

  for (int i = 0; i < 5; i++) {
    assert(get_version(u[i]) == 0x7);
    assert(get_variant(u[i]) == 0x2);
    printf("UUID[%d] version=0x%x variant=0x%x OK\n", i, get_version(u[i]),
           get_variant(u[i]));
  }
}

static void test_timestamp_validity(void) {
  uuid7_ctx ctx;
  uuid7_init(&ctx);
  uint8_t u[16];
  uuid7_generate(&ctx, u);

  uint64_t ts = get_timestamp(u);
  assert(ts > 1577836800000ULL);
  printf("Timestamp: %llu ms (OK, > 2020-01-01)\n", (unsigned long long)ts);
}

static void test_no_version_variant_corruption(void) {
  uuid7_ctx ctx;
  uuid7_init(&ctx);
  uint8_t u[16];

  // Paksa rand_a_high dan counter bernilai yang bisa membahayakan masking
  ctx.rand_a_high = 0xF;     // nibble penuh, pastikan tidak tumpah ke versi
  ctx.counter_high = 0x3FFF; // semua bit 1, pastikan variant aman
  ctx.counter_low = 0x00FFFFFFFFFFFFFFULL;

  uuid7_generate_ex(&ctx, u, ctx.last_timestamp_ms + 1);

  assert(get_version(u) == 0x7);
  assert(get_variant(u) == 0x2);
  printf("Variant aman walau rand_a dan counter maksimum.\n");
}

static void test_big_endian_timestamp(void) {
  uuid7_ctx ctx;
  uuid7_init(&ctx);

  uint64_t specific_ts = 0x0001A2B3C4D5EULL; // 48-bit
  specific_ts &= 0xFFFFFFFFFFFFULL;

  uint8_t u[16];
  uuid7_generate_ex(&ctx, u, specific_ts);

  // Byte 0-5 harus big endian dari specific_ts
  uint64_t extracted = get_timestamp(u);
  assert(extracted == specific_ts);
  printf("Timestamp big-endian benar: 0x%012llx\n",
         (unsigned long long)extracted);

  // Verifikasi manual byte order
  assert(u[0] == (uint8_t)(specific_ts >> 40));
  assert(u[1] == (uint8_t)(specific_ts >> 32));
  assert(u[2] == (uint8_t)(specific_ts >> 24));
  assert(u[3] == (uint8_t)(specific_ts >> 16));
  assert(u[4] == (uint8_t)(specific_ts >> 8));
  assert(u[5] == (uint8_t)(specific_ts));
}

static void test_monotonic_edge_cases(void) {
  uuid7_ctx ctx;
  uuid7_init(&ctx);
  uint8_t prev[16], curr[16];
  int violations = 0;

  // Skenario: timestamp tetap, counter penuh
  ctx.last_timestamp_ms = 1000;
  ctx.counter_low = 0x00FFFFFFFFFFFFFFFEULL;
  ctx.counter_high = 0;
  ctx.rand_a_high = 0;

  uuid7_generate_ex(&ctx, prev, 1000);
  uuid7_generate_ex(&ctx, curr, 1000); // overflow 56-bit
  if (memcmp(prev, curr, 16) >= 0)
    violations++;
  memcpy(prev, curr, 16);

  uuid7_generate_ex(&ctx, curr, 995); // mundur 5ms, masih dalam threshold
  if (memcmp(prev, curr, 16) >= 0)
    violations++;
  memcpy(prev, curr, 16);

  uuid7_generate_ex(&ctx, curr, 1001);
  if (memcmp(prev, curr, 16) >= 0)
    violations++;

  assert(violations == 0);
  printf(
      "Monotonisitas terjaga dalam edge case (counter overflow + mundur).\n");
}

int main(void) {
  printf("RFC 9562 Compliance Tests\n");
  printf("==========================\n\n");

  printf("1. Versi & variant konsisten:\n");
  test_version_variant_consistency();

  printf("\n2. Validitas timestamp:\n");
  test_timestamp_validity();

  printf("\n3. Perlindungan versi/variant:\n");
  test_no_version_variant_corruption();

  printf("\n4. Big endian timestamp:\n");
  test_big_endian_timestamp();

  printf("\n5. Monotonisitas edge case:\n");
  test_monotonic_edge_cases();

  printf("\nSemua test RFC 9562 selesai. Library comply dengan standar.\n");
  return 0;
}