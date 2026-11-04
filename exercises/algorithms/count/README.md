[English](README.md) | [Русский](README.ru.md)

# `bys::count`

Implement counting in [include/bys/count.hpp](include/bys/count.hpp).

- [`std::count` on cppreference](https://en.cppreference.com/cpp/algorithm/count)
- [`std::count` in the C++20 draft standard](https://timsong-cpp.github.io/cppwp/n4861/alg.count)

**Input.** Two input iterators `first` and `last` defining the range `[first, last)`, and a value to count.

**Output.** The number of elements for which `*iterator == value` is true, returned as the iterator's `difference_type`.

## Build and run tests

After implementing `bys::count`, run these commands from the project root.

```sh
cmake -S . -B out/count
cmake --build out/count --target count_tests
./out/count/exercises/algorithms/count/count_tests
```
