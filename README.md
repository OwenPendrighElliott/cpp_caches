# C++ LRU and LFU Cache

The is a header only implementation of LRU and LFU cache in C++ for use in other projects.

The LFU cache uses an efficient implementations with the LFU cache giving O(1) time complexity for all operations.

## Tests

Initialise the submodules with the following command:
```bash
git submodule update --init --recursive
```

Build and run the tests with the following command:
```bash
rm -rf build && cmake -B build -S . && cmake --build build -j 8
./build/test_lru
./build/test_lfu
```