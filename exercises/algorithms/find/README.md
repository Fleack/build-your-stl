[English](README.md) | [Русский](README.ru.md)

# `bys::find`

Implement linear search in [include/bys/find.hpp](include/bys/find.hpp).

- [`std::find` on cppreference](https://en.cppreference.com/cpp/algorithm/find)
- [`std::find` in the C++20 draft standard](https://timsong-cpp.github.io/cppwp/n4861/alg.find)

**Input.** Two input iterators `first` and `last` defining the range `[first, last)`, and a value to find.

**Output.** An iterator to the first element for which `*iterator == value` is true, or `last` if no element matches.

## Build and run tests

After implementing `bys::find`, run these commands from the project root.

```sh
cmake -S . -B out/find
cmake --build out/find --target find_tests
./out/find/exercises/algorithms/find/find_tests
```
