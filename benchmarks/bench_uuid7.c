#include "uuid7.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static uint64_t get_time_ms(void) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (uint64_t)ts.tv_sec * 1000 + (uint64_t)ts.tv_nsec / 1000000;
}

static void bench_throughput_heap(int count) {
  uuid7_ctx *ctx = uuid7_create();
  uint8_t buf[16];

  uint64_t start = get_time_ms();
  for (int i = 0; i < count; i++) {
    uuid7_generate(ctx, buf);
  }
  uint64_t end = get_time_ms();

  double elapsed = (end - start) / 1000.0;
  double rate = count / elapsed;
  printf("[HEAP] %d UUID dalam %.9f s -> %.0f UUID/s (%.1f ns/UUID)\n", count,
         elapsed, rate, 1e9 / rate);

  uuid7_destroy(ctx);
}

static void bench_throughput_stack(int count) {
  uuid7_ctx ctx;
  uuid7_init(&ctx);
  uint8_t buf[16];

  uint64_t start = get_time_ms();
  for (int i = 0; i < count; i++) {
    uuid7_generate(&ctx, buf);
  }
  uint64_t end = get_time_ms();

  double elapsed = (end - start) / 1000.0;
  double rate = count / elapsed;
  printf("[STACK] %d UUID dalam %.9f s -> %.0f UUID/s (%.1f ns/UUID)\n", count,
         elapsed, rate, 1e9 / rate);
}

static void bench_scalar_match(int count) {
  uuid7_ctx ctx;
  uuid7_init(&ctx);

  uint8_t (*uuids)[16] = malloc(count * 16);
  if (!uuids) {
    printf("Gagal alokasi memori untuk scalar match test.\n");
    return;
  }

  for (int i = 0; i < count; i++) {
    uuid7_generate(&ctx, uuids[i]);
  }

  uint64_t start = get_time_ms();
  int violations = 0;
  for (int i = 0; i < count - 1; i++) {
    if (memcmp(uuids[i], uuids[i + 1], 16) >= 0) {
      violations++;
    }
  }
  uint64_t end = get_time_ms();

  double elapsed = (end - start) / 1000.0;
  printf("[SCALAR MATCH] %d UUID: %d pelanggaran monotonisitas\n", count,
         violations);
  printf("Waktu validasi %d memcmp: %.9f s\n", count - 1, elapsed);
  printf("-> Scalar match %s\n", violations == 0 ? "TERVERIFIKASI" : "GAGAL");

  free(uuids);
}

int main(void) {
  printf("\nUUID v7 Benchmark\n");
  printf("=================\n\n");

  int n = 5000000; // 5 juta

  printf("1. Throughput:\n");
  bench_throughput_heap(n);
  bench_throughput_stack(n);

  printf("\n2. Scalar match validation:\n");
  bench_scalar_match(1000000);

  return 0;
}