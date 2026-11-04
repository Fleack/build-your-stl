[English](README.md) | [Русский](README.ru.md)

# `bys::span`

Implement the C++20 `span` in [include/bys/span.hpp](include/bys/span.hpp).

- [`std::span` on cppreference](https://en.cppreference.com/cpp/container/span)
- [`std::span` in the C++20 draft standard](https://timsong-cpp.github.io/cppwp/n4861/views.span)
- [C++20 correction for views with a fixed extent (P2325R3)](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p2325r3.html)

**Input.** An element type, a fixed extent or `bys::dynamic_extent`, and a contiguous sequence supplied as an array, a sized range, another span, or an iterator with a count or a sized sentinel.

**Result.** A view that refers to the original elements without owning them. Match C++20 construction, deduction, conversions, element access, iteration, size queries and subviews. Support the ranges concepts `view` and `borrowed_range`, and provide `bys::as_bytes` and `bys::as_writable_bytes`.

## Build and run tests

After implementing `bys::span`, run these commands from the project root.

```sh
cmake -S . -B out/span
cmake --build out/span --target span_tests
./out/span/exercises/views/span/span_tests
```
