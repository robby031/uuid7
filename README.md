**Run Test**
```bash
cmake -B build -DBUILD_TESTS=ON
cmake --build build
ctest --test-dir build          
./build/tests/test_stress
./build/tests/test_clock_backward
./build/tests/test_rfc9562
./build/tests/test_uuid7
```

**Run Benchmarks**
```bash
cmake -B build -DBUILD_BENCHMARKS=ON
cmake --build build
./build/benchmarks/bench_compare
./build/benchmarks/bench_uuid7
```