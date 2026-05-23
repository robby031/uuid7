#include "uuid7.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int tests_run = 0;
static int tests_passed = 0;

#define TEST(name)                                                             \
  void name(void);                                                             \
  struct {                                                                     \
    void (*fn)(void);                                                          \
    const char *name;                                                          \
  } test_##name = {name, #name};
#define RUN_TEST(name)                                                         \
  do {                                                                         \
    printf("Running %s...\n", #name);                                          \
    tests_run++;                                                               \
    name();                                                                    \
    tests_passed++;                                                            \
  } while (0)

static void print_uuid(const uint8_t *uuid) {
  for (int i = 0; i < 16; i++) {
    printf("%02x", uuid[i]);
  }
  printf("\n");
}

static int uuid_cmp(const uint8_t *a, const uint8_t *b) {
  return memcmp(a, b, 16);
}

void test_version_variant(void) {
  // Alokasi heap
  uuid7_ctx *ctx = uuid7_create();
  assert(ctx != NULL);

  uint8_t uuid[16];
  uuid7_generate(ctx, uuid);
  uuid7_destroy(ctx);

  assert((uuid[6] >> 4) == 0x7);
  assert((uuid[8] >> 6) == 0x2);

  printf("  UUID: ");
  print_uuid(uuid);
  printf("  Version: %x\n", uuid[6] >> 4);
  printf("  Variant: %x\n", uuid[8] >> 6);
}

void test_stack_allocation(void) {
  uuid7_ctx ctx;
  uuid7_init(&ctx);

  uint8_t uuid1[16], uuid2[16];
  uuid7_generate(&ctx, uuid1);
  uuid7_generate(&ctx, uuid2);

  assert(uuid_cmp(uuid1, uuid2) < 0);

  assert((uuid1[6] >> 4) == 0x7);
  assert((uuid2[6] >> 4) == 0x7);
  assert((uuid1[8] >> 6) == 0x2);
  assert((uuid2[8] >> 6) == 0x2);

  printf("  Stack UUID1: ");
  print_uuid(uuid1);
  printf("  Stack UUID2: ");
  print_uuid(uuid2);
}

void test_monotonic_same_ms(void) {
  uuid7_ctx ctx;
  uuid7_init(&ctx);

#define N 10000
  uint8_t *uuids = malloc(N * 16);
  assert(uuids != NULL);
  for (int i = 0; i < N; i++) {
    uuid7_generate(&ctx, uuids + i * 16);
  }

  for (int i = 0; i < N - 1; i++) {
    assert(uuid_cmp(uuids + i * 16, uuids + (i + 1) * 16) < 0);
  }

  free(uuids);
  printf("Generated %d UUIDs, all monotonic and unique.\n", N);
}

void test_heap_allocation(void) {
  uuid7_ctx *ctx = uuid7_create();
  assert(ctx != NULL);

  uint8_t uuid1[16], uuid2[16];
  uuid7_generate(ctx, uuid1);
  uuid7_generate(ctx, uuid2);

  assert(uuid_cmp(uuid1, uuid2) < 0);
  assert((uuid1[6] >> 4) == 0x7);
  assert((uuid1[8] >> 6) == 0x2);

  uuid7_destroy(ctx);
}

void test_multiple_contexts_independent(void) {
  uuid7_ctx ctx1, ctx2;
  uuid7_init(&ctx1);
  uuid7_init(&ctx2);

  uint8_t a1[16], a2[16], b1[16], b2[16];

  uuid7_generate(&ctx1, a1);
  uuid7_generate(&ctx1, a2);
  uuid7_generate(&ctx2, b1);
  uuid7_generate(&ctx2, b2);

  assert(uuid_cmp(a1, a2) < 0);
  assert(uuid_cmp(b1, b2) < 0);
}

void test_no_duplicate_high_volume(void) {
  uuid7_ctx ctx;
  uuid7_init(&ctx);

#define M 100000
  uint8_t (*uuids)[16] = malloc(M * 16);
  assert(uuids != NULL);

  for (int i = 0; i < M; i++) {
    uuid7_generate(&ctx, uuids[i]);
  }

  for (int i = 0; i < M - 1; i++) {
    if (uuid_cmp(uuids[i], uuids[i + 1]) >= 0) {
      printf("Error at %d: ", i);
      print_uuid(uuids[i]);
      print_uuid(uuids[i + 1]);
      assert(0);
    }
  }

  free(uuids);
  printf("Generated %d UUIDs, all monotonic and unique.\n", M);
}

int main(void) {
  printf("\nUUID v7 Unit Tests\n");
  printf("==================\n");

  RUN_TEST(test_version_variant);
  RUN_TEST(test_stack_allocation);
  RUN_TEST(test_monotonic_same_ms);
  RUN_TEST(test_heap_allocation);
  RUN_TEST(test_multiple_contexts_independent);
  RUN_TEST(test_no_duplicate_high_volume);

  printf("\nAll %d tests passed (%d total).\n", tests_passed, tests_run);
  return 0;
}