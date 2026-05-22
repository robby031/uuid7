#ifndef UUID7_INTERNAL_H
#define UUID7_INTERNAL_H

#include <stdint.h>
#include <stddef.h>

#ifndef UUID7_CLOCK_BACKWARD_THRESHOLD_MS
#define UUID7_CLOCK_BACKWARD_THRESHOLD_MS 10000
#endif

uint64_t os_time_ms(void);

#include "uuid7.h"

void uuid7_generate_ex(uuid7_ctx *ctx, uint8_t out[16], uint64_t timestamp_ms);

void prng_seed(uuid7_prng *prng);

/**
 * Isi buffer dengan data acak dari PRNG.
 * @param prng Pointer ke state PRNG.
 * @param buf  Buffer tujuan.
 * @param len  Jumlah byte yang akan diisi (maksimum 16 disarankan, tapi implementasi bisa lebih).
 */
void prng_fill(uuid7_prng *prng, void *buf, size_t len);

#endif // UUID7_INTERNAL_H