[English](README.md) | [Русский](README.ru.md)

# `bys::equal`

Implement element-by-element comparison of two ranges in [include/bys/equal.hpp](include/bys/equal.hpp).

- [`std::equal` on cppreference](https://en.cppreference.com/cpp/algorithm/equal)
- [`std::equal` in the C++20 draft standard](https://timsong-cpp.github.io/cppwp/n4861/alg.equal)

**Input.** Four input iterators `first1`, `last1`, `first2`, and `last2` defining the ranges `[first1, last1)` and `[first2, last2)`.

**Output.** `true` if the ranges have the same length and every pair of corresponding elements compares equal with `==`. Otherwise, `false`.

## Build and run tests

After implementing `bys::equal`, run these commands from the project root.

```sh
cmake -S . -B out/equal
cmake --build out/equal --target equal_tests
./out/equal/exercises/algorithms/equal/equal_tests
```
