import threading

from ._ffi import UUID7Generator

_lock = threading.Lock()
_default_gen: UUID7Generator | None = None


def _default() -> UUID7Generator:
    global _default_gen
    if _default_gen is None:
        with _lock:
            if _default_gen is None:
                _default_gen = UUID7Generator()
    return _default_gen


def generate() -> bytes:
    """Buat UUID v7 menggunakan generator bawaan. Tidak perlu inisialisasi manual."""
    return _default().generate()


def generate_str() -> str:
    """Buat UUID v7 dan kembalikan string berformat xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx."""
    h = generate().hex()
    return f"{h[0:8]}-{h[8:12]}-{h[12:16]}-{h[16:20]}-{h[20:32]}"


__all__ = ["UUID7Generator", "generate", "generate_str"]
