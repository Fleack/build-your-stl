[English](README.md) | [Русский](README.ru.md)

# `bys::integral_constant`

Implement a class template that represents a compile-time value as a type in [include/bys/integral_constant.hpp](include/bys/integral_constant.hpp).

- [`std::integral_constant` on cppreference](https://en.cppreference.com/cpp/types/integral_constant)
- [`std::integral_constant` in the C++20 draft standard](https://timsong-cpp.github.io/cppwp/n4861/meta.help)

The template takes a type `T` and a value `v` of type `T`. Support any `T` allowed for a non-type template parameter in C++20.

## Build and run tests

After implementing the exercise, run these commands from the project root.

```sh
cmake -S . -B out/integral_constant
cmake --build out/integral_constant --target integral_constant_tests
./out/integral_constant/exercises/type_traits/integral_constant/integral_constant_tests
```
