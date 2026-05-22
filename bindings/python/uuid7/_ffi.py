import ctypes
import os
import platform

def _get_lib_name():
    system = platform.system()
    if system == "Linux":
        return "libuuid7.so"
    elif system == "Darwin":
        return "libuuid7.dylib"
    elif system == "Windows":
        return "uuid7.dll"
    else:
        raise OSError(f"Unsupported platform: {system}")

_lib_path = os.path.join(os.path.dirname(__file__), _get_lib_name())
if not os.path.exists(_lib_path):
    _lib_path = _get_lib_name()

try:
    _lib = ctypes.CDLL(_lib_path)
except OSError as e:
    raise OSError(
        f"Tidak dapat memuat library uuid7: {_lib_path}. "
        "Pastikan library sudah di build"
    ) from e

class _UUID7_CTX(ctypes.Structure):
    _fields_ = [
        ("last_timestamp_ms", ctypes.c_uint64),
        ("rand_a_high", ctypes.c_uint8),
        ("counter_high", ctypes.c_uint16),
        ("counter_low", ctypes.c_uint64),
        ("prng", ctypes.c_uint64 * 4),
        ("_reserved", ctypes.c_uint64 * 4),
    ]

_lib.uuid7_create.restype = ctypes.POINTER(_UUID7_CTX)
_lib.uuid7_destroy.argtypes = [ctypes.POINTER(_UUID7_CTX)]
_lib.uuid7_init.argtypes = [ctypes.POINTER(_UUID7_CTX)]
_lib.uuid7_generate.argtypes = [ctypes.POINTER(_UUID7_CTX), ctypes.c_char_p]

class UUID7Generator:
    def __init__(self, heap=True):
        self._heap = heap
        if heap:
            self._ctx = _lib.uuid7_create()
            if not self._ctx:
                raise MemoryError("Gagal membuat konteks uuid7")
            self._stack_buf = None
        else:
            self._stack_buf = ctypes.create_string_buffer(ctypes.sizeof(_UUID7_CTX))
            self._ctx = ctypes.cast(self._stack_buf, ctypes.POINTER(_UUID7_CTX))
            _lib.uuid7_init(self._ctx)

    def generate(self) -> bytes:
        buf = ctypes.create_string_buffer(16)
        _lib.uuid7_generate(self._ctx, buf)
        return buf.raw

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.close()
        return False

    def close(self):
        if not self._ctx:
            return
        if self._heap:
            _lib.uuid7_destroy(self._ctx)
        self._ctx = None
        self._stack_buf = None

    def __del__(self):
        self.close()