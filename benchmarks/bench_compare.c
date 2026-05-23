#include "uuid7.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../src/internal.h"

static uint64_t get_time_ms(void) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (uint64_t)ts.tv_sec * 1000 + (uint64_t)ts.tv_nsec / 1000000;
}

static void generate_uuid_v4(uuid7_prng *prng, uint8_t out[16]) {
  uint64_t rnd[2];
  prng_fill(prng, rnd, 16);
  memcpy(out, rnd, 16);
  out[6] = 0x40 | (out[6] & 0x0F); // versi 4
  out[8] = 0x80 | (out[8] & 0x3F); // variant 10xx
}

static void bench_throughput_compare(int count) {
  printf("1. Throughput Generate (%d UUID):\n", count);

  uuid7_ctx ctx7;
  uuid7_init(&ctx7);
  uint8_t buf[16];
  uint64_t start = get_time_ms();
  for (int i = 0; i < count; i++) {
    uuid7_generate(&ctx7, buf);
  }
  uint64_t end = get_time_ms();
  double elapsed7 = (end - start) / 1000.0;
  printf("UUID v7 : %.9f s -> %.0f UUID/s\n", elapsed7, count / elapsed7);

  uuid7_prng prng4;
  prng_seed(&prng4);
  start = get_time_ms();
  for (int i = 0; i < count; i++) {
    generate_uuid_v4(&prng4, buf);
  }
  end = get_time_ms();
  double elapsed4 = (end - start) / 1000.0;
  printf("UUID v4 : %.9f s -> %.0f UUID/s\n", elapsed4, count / elapsed4);
}

static int uuid_compare(const void *a, const void *b) {
  return memcmp(a, b, 16);
}

static void bench_sorting(int count) {
  printf("\n2. Sorting Performance (%d UUID):\n", count);

  uint8_t (*uuids_v7)[16] = malloc(count * 16);
  uint8_t (*uuids_v4)[16] = malloc(count * 16);
  if (!uuids_v7 || !uuids_v4) {
    printf("Gagal alokasi memori.\n");
    free(uuids_v7);
    free(uuids_v4);
    return;
  }

  uuid7_ctx ctx7;
  uuid7_init(&ctx7);
  for (int i = 0; i < count; i++) {
    uuid7_generate(&ctx7, uuids_v7[i]);
  }

  uuid7_prng prng4;
  prng_seed(&prng4);
  for (int i = 0; i < count; i++) {
    generate_uuid_v4(&prng4, uuids_v4[i]);
  }

  uint64_t start = get_time_ms();
  qsort(uuids_v7, count, 16, uuid_compare);
  uint64_t end = get_time_ms();
  double elapsed7 = (end - start) / 1000.0;
  printf("Sort UUID v7 (monotonik) : %.9f s\n", elapsed7);

  start = get_time_ms();
  qsort(uuids_v4, count, 16, uuid_compare);
  end = get_time_ms();
  double elapsed4 = (end - start) / 1000.0;
  printf("Sort UUID v4 (acak)      : %.9f s\n", elapsed4);

  if (elapsed7 < elapsed4) {
    printf("-> UUID v7 %.1fx lebih cepat di sort.\n", elapsed4 / elapsed7);
  } else {
    printf("-> UUID v7 lebih lambat di sort.\n");
  }

  free(uuids_v7);
  free(uuids_v4);
}

static void bench_insertion(int count) {
  printf("\n3. Simulasi Insertion ke B-Tree (%d UUID):\n", count);

  uuid7_ctx ctx7;
  uuid7_init(&ctx7);
  uint8_t *arr7 = malloc(count * 16);
  if (!arr7)
    return;
  uint64_t start = get_time_ms();
  for (int i = 0; i < count; i++) {
    uuid7_generate(&ctx7, &arr7[i * 16]);
  }
  uint64_t end = get_time_ms();
  double elapsed7 = (end - start) / 1000.0;
  printf("UUID v7 (langsung append) : %.9f s\n", elapsed7);

  uint8_t *arr4 = malloc(count * 16);
  if (!arr4) {
    free(arr7);
    return;
  }
  uuid7_prng prng4;
  prng_seed(&prng4);
  int n = 0;
  start = get_time_ms();
  for (int i = 0; i < count; i++) {
    uint8_t new_uuid[16];
    generate_uuid_v4(&prng4, new_uuid);

    int left = 0, right = n;
    while (left < right) {
      int mid = left + (right - left) / 2;
      if (memcmp(new_uuid, &arr4[mid * 16], 16) > 0) {
        left = mid + 1;
      } else {
        right = mid;
      }
    }

    if (left < n) {
      memmove(&arr4[(left + 1) * 16], &arr4[left * 16], (n - left) * 16);
    }
    memcpy(&arr4[left * 16], new_uuid, 16);
    n++;
  }
  end = get_time_ms();
  double elapsed4 = (end - start) / 1000.0;
  printf("UUID v4 (cari posisi + geser) : %.9f s\n", elapsed4);
  printf("-> UUID v7 %.1fx lebih efisien untuk insertion.\n",
         elapsed4 / elapsed7);

  free(arr7);
  free(arr4);
}

int main(void) {
  printf("\nUUID v7 vs v4 Comparison Benchmark\n");
  printf("==================================\n\n");

  int n_gen = 5000000;    // 5 juta untuk throughput
  int n_sort = 5000000;   // 5 juta untuk sorting
  int n_insert = 1000000; // 1 juta untuk insertion

  bench_throughput_compare(n_gen);
  bench_sorting(n_sort);
  bench_insertion(n_insert);

  return 0;
}