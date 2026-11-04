[English](README.md) | [Русский](README.ru.md)

# `bys::upper_bound`

Implement both classic `upper_bound` overloads in [include/bys/upper_bound.hpp](include/bys/upper_bound.hpp).

- [`std::upper_bound` on cppreference](https://en.cppreference.com/cpp/algorithm/upper_bound)
- [`std::upper_bound` in the C++20 draft standard](https://timsong-cpp.github.io/cppwp/n4861/upper.bound)

**Input.** Two forward iterators `first` and `last`, a search value, and, in the second overload, a comparator `comp`. In `[first, last)`, all elements for which `std::less<>{}(value, element)` or `comp(value, element)` is false precede those for which it is true.

**Output.** An iterator to the first element for which the comparison is true, or `last` if there is no such element.

## Build and run tests

After implementing `bys::upper_bound`, run these commands from the project root.

```sh
cmake -S . -B out/upper_bound
cmake --build out/upper_bound --target upper_bound_tests
./out/upper_bound/exercises/algorithms/upper_bound/upper_bound_tests
```
