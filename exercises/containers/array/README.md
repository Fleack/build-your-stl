[English](README.md) | [Русский](README.ru.md)

# `bys::array`

Implement the C++20 fixed-size container in [include/bys/array.hpp](include/bys/array.hpp).

- [`std::array` on cppreference](https://en.cppreference.com/cpp/container/array)
- [`std::array` in the C++20 draft standard](https://timsong-cpp.github.io/cppwp/n4861/array)

**Input.** An element type `T`, a compile-time size `N` that may be zero, and up to `N` initializers.

**Result.** An aggregate `bys::array<T, N>` that stores exactly `N` elements contiguously. Match the C++20 member types and operations for capacity, element access, iteration, `fill`, and `swap`. Include deduction, comparisons, the free `bys::swap` and `bys::to_array` functions, and tuple support through `bys::get`, `std::tuple_size`, and `std::tuple_element`.

## Build and run tests

After implementing `bys::array`, run these commands from the project root.

```sh
cmake -S . -B out/array
cmake --build out/array --target array_tests
./out/array/exercises/containers/array/array_tests
```
