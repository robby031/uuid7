#include "internal.h"
#include "uuid7.h"
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_THREADS 32
#define UUIDS_PER_THREAD 500000 // Total 16 juta UUIDs across all threads
#define TOTAL_UUIDS (NUM_THREADS * UUIDS_PER_THREAD)

static int uuid_cmp(const void *a, const void *b) { return memcmp(a, b, 16); }

static int uuid_cmp_inline(const uint8_t *a, const uint8_t *b) {
  return memcmp(a, b, 16);
}

static void check_version_variant(const uint8_t *uuid) {
  assert((uuid[6] >> 4) == 0x7);
  assert((uuid[8] >> 6) == 0x2);
}

typedef struct {
  int thread_id;
  uint8_t *output;
} thread_arg;

static void *thread_generate(void *arg) {
  thread_arg *ta = arg;
  uuid7_ctx ctx;
  uuid7_init(&ctx);

  uint8_t *base = ta->output;
  uuid7_generate(&ctx, base);
  check_version_variant(base);

  for (int i = 1; i < UUIDS_PER_THREAD; i++) {
    uint8_t *curr = base + i * 16;
    uint8_t *prev = base + (i - 1) * 16;
    uuid7_generate(&ctx, curr);
    check_version_variant(curr);

    if (uuid_cmp_inline(prev, curr) >= 0) {
      fprintf(stderr, "[thread %d] monotonicity break at i=%d\n", ta->thread_id,
              i);
      assert(0);
    }
  }
  return NULL;
}

int main(void) {
  printf("\nUUID v7 Multithreaded Stress Test\n");
  printf("====================================\n");
  printf("Threads : %d\n", NUM_THREADS);
  printf("Per thread: %d UUIDs\n", UUIDS_PER_THREAD);
  printf("Total   : %d UUIDs\n\n", TOTAL_UUIDS);

  uint8_t *all_uuids = malloc(TOTAL_UUIDS * 16);
  assert(all_uuids != NULL);

  pthread_t threads[NUM_THREADS];
  thread_arg args[NUM_THREADS];

  for (int t = 0; t < NUM_THREADS; t++) {
    args[t].thread_id = t;
    args[t].output = all_uuids + t * UUIDS_PER_THREAD * 16;
    pthread_create(&threads[t], NULL, thread_generate, &args[t]);
  }

  for (int t = 0; t < NUM_THREADS; t++) {
    pthread_join(threads[t], NULL);
  }
  printf("Per-thread monotonicity: PASSED\n");

  printf("Sorting %d UUIDs for global collision check...\n", TOTAL_UUIDS);
  qsort(all_uuids, TOTAL_UUIDS, 16, uuid_cmp);

  int duplicates = 0;
  for (int i = 1; i < TOTAL_UUIDS; i++) {
    if (uuid_cmp_inline(all_uuids + (i - 1) * 16, all_uuids + i * 16) == 0) {
      duplicates++;
    }
  }

  if (duplicates) {
    fprintf(stderr, "COLLISION DETECTED: %d duplicates\n", duplicates);
    free(all_uuids);
    return 1;
  }

  printf("Global uniqueness      : PASSED (no duplicates)\n");
  printf("\nAll %d UUIDs generated across %d threads OK.\n", TOTAL_UUIDS,
         NUM_THREADS);

  free(all_uuids);
  return 0;
}
