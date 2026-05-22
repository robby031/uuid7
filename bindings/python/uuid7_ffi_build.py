import os
from cffi import FFI

ffi = FFI()

ffi.cdef("""
    typedef struct { uint64_t s[4]; } uuid7_prng;

    typedef struct {
        uint64_t   last_timestamp_ms;
        uint8_t    rand_a_high;
        uint16_t   counter_high;
        uint64_t   counter_low;
        uuid7_prng prng;
        uint64_t   _reserved[4];
    } uuid7_ctx;

    uuid7_ctx* uuid7_create(void);
    void       uuid7_destroy(uuid7_ctx *ctx);
    void       uuid7_init(uuid7_ctx *ctx);
    void       uuid7_generate(uuid7_ctx *ctx, uint8_t *out);
""")

_csrc = os.path.join(os.path.dirname(__file__), "uuid7", "csrc")

ffi.set_source(
    "uuid7._libuuid7",
    '#include "uuid7.h"',
    sources=[
        os.path.join(_csrc, "uuid7_core.c"),
        os.path.join(_csrc, "clock.c"),
        os.path.join(_csrc, "prng.c"),
    ],
    include_dirs=[_csrc],
)

if __name__ == "__main__":
    ffi.compile(verbose=True)
