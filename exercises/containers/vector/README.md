[English](README.md) | [Русский](README.ru.md)

# `bys::vector`

Implement the primary C++20 `vector` template in [include/bys/vector.hpp](include/bys/vector.hpp). The packed `vector<bool>` specialization is outside the scope of this exercise.

- [`std::vector` on cppreference](https://en.cppreference.com/cpp/container/vector)
- [`std::vector` in the C++20 draft standard](https://timsong-cpp.github.io/cppwp/n4861/vector)

**Input.** An element type `T`, an optional allocator type, and initial elements given as a count, a count and value, an iterator range, or an initializer list. A vector can also start empty or be copied or moved from another vector.

**Result.** A resizable container that owns a contiguous sequence. Match the C++20 member types, deduction, construction, assignment, element access, iteration, capacity, modifiers, allocator behavior and comparisons. Provide the free functions `bys::swap`, `bys::erase` and `bys::erase_if`.

## Build and run tests

After implementing `bys::vector`, run these commands from the project root.

```sh
cmake -S . -B out/vector
cmake --build out/vector --target vector_tests
./out/vector/exercises/containers/vector/vector_tests
```
