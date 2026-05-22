#ifndef UUID7_H
#define UUID7_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UUID7_API_VERSION 1

typedef struct {
    uint64_t s[4];
} uuid7_prng;

/*
 * ABI STABILITY POLICY
 * --------------------
 * sizeof(uuid7_ctx) tetap konstan selama _reserved masih tersisa.
 * Field tanpa prefiks _ adalah public contract dan tidak akan diubah
 * posisi/ukurannya pada UUID7_API_VERSION yang sama.
 * Field _reserved hanya untuk internal ekspansi masa depan;
 * jangan akses langsung dari kode pengguna.
 *
 * Jika padding habis dan perlu restructure struct, UUID7_API_VERSION
 * akan naik dan ini akan menjadi breaking change yang diumumkan
 * secara eksplisit.
 */
typedef struct {
    uint64_t   last_timestamp_ms;   // timestamp Unix milidetik
    uint8_t    rand_a_high;         // 4 bit nibble tetap per ms
    uint16_t   counter_high;        // 14 bit counter bagian atas
    uint64_t   counter_low;         // 56 bit counter bagian bawah
    uuid7_prng prng;                // state PRNG
    uint64_t   _reserved[4];        // 32 byte untuk ekspansi ABI
} uuid7_ctx;

uuid7_ctx* uuid7_create(void);

void uuid7_destroy(uuid7_ctx *ctx);

void uuid7_init(uuid7_ctx *ctx);

void uuid7_generate(uuid7_ctx *ctx, uint8_t out[16]);

#ifdef __cplusplus
}
#endif

#endif // UUID7_H