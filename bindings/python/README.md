# UUID v7 - Python

Binding Python untuk library UUID v7.

## Install

```bash
cd bindings/python
pip install .
```

## Penggunaan

```python
from uuid7 import UUID7Generator

# Heap allocation (default)
gen = UUID7Generator()
uuid = gen.generate()
print(uuid.hex())

# Stack allocation (zero overhead)
gen = UUID7Generator(heap=False)
uuid = gen.generate()
print(uuid.hex())
```
