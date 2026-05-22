import uuid7
import time

def test_generate_stack():
    gen = uuid7.UUID7Generator(heap=False)
    u1 = gen.generate()
    u2 = gen.generate()
    assert len(u1) == 16
    assert len(u2) == 16
    assert u1 != u2
    assert u1 < u2 

    assert (u1[6] >> 4) == 0x7
    assert (u1[8] >> 6) == 0x2
    gen.close()

def test_generate_heap():
    with uuid7.UUID7Generator(heap=True) as gen:
        u1 = gen.generate()
        u2 = gen.generate()
        assert u1 < u2

def test_monotonic_volume():
    gen = uuid7.UUID7Generator(heap=False)
    prev = gen.generate()
    for _ in range(1000):
        curr = gen.generate()
        assert prev < curr
        prev = curr
    gen.close()

def test_scalar_match():
    """Pastikan urutan byte sesuai urutan waktu."""
    gen = uuid7.UUID7Generator(heap=False)
    uuids = [gen.generate() for _ in range(1000)]
    
    assert all(uuids[i] < uuids[i+1] for i in range(len(uuids)-1))
    gen.close()

if __name__ == "__main__":
    test_generate_stack()
    test_generate_heap()
    test_monotonic_volume()
    test_scalar_match()
    print("Semua test Python berhasil.")