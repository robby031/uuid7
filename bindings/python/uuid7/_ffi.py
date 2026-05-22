from ._libuuid7 import ffi, lib


class UUID7Generator:
    def __init__(self, heap=True):
        self._heap = heap
        if heap:
            self._ctx = lib.uuid7_create()
            if self._ctx == ffi.NULL:
                raise MemoryError("Gagal membuat konteks uuid7")
        else:
            self._ctx = ffi.new("uuid7_ctx *")
            lib.uuid7_init(self._ctx)

    def generate(self) -> bytes:
        buf = ffi.new("uint8_t[16]")
        lib.uuid7_generate(self._ctx, buf)
        return bytes(ffi.buffer(buf))

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.close()
        return False

    def close(self):
        if self._ctx is None:
            return
        if self._heap and self._ctx != ffi.NULL:
            lib.uuid7_destroy(self._ctx)
        self._ctx = None

    def __del__(self):
        self.close()
