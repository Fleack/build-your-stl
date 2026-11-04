[English](README.md) | [Русский](README.ru.md)

# `bys::lower_bound`

Implement both classic `lower_bound` overloads in [include/bys/lower_bound.hpp](include/bys/lower_bound.hpp).

- [`std::lower_bound` on cppreference](https://en.cppreference.com/cpp/algorithm/lower_bound)
- [`std::lower_bound` in the C++20 draft standard](https://timsong-cpp.github.io/cppwp/n4861/lower.bound)

**Input.** Two forward iterators `first` and `last`, a search value, and, in the second overload, a comparator `comp`. The range `[first, last)` is partitioned by `std::less<>{}(element, value)` or `comp(element, value)`: all elements for which the comparison is true precede those for which it is false.

**Output.** An iterator to the first element for which the comparison is false, or `last` if it is true for every element.

## Build and run tests

After implementing `bys::lower_bound`, run these commands from the project root.

```sh
cmake -S . -B out/lower_bound
cmake --build out/lower_bound --target lower_bound_tests
./out/lower_bound/exercises/algorithms/lower_bound/lower_bound_tests
```
