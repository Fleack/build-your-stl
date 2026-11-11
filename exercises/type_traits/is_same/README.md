[English](README.md) | [Русский](README.ru.md)

# `bys::is_same`

Implement `bys::is_same<T, U>` and `bys::is_same_v<T, U>` in [include/bys/is_same.hpp](include/bys/is_same.hpp).

Both report at compile time whether `T` and `U` are identical types. A type alias denotes the same type. Differences in `const`, `volatile`, or reference qualifiers do matter.

`is_same<T, U>` exposes the interface of a boolean constant. Its result is available as `::value`, through implicit conversion to `bool`, and by calling the object.

## Build and run tests

```sh
cmake -S . -B out/is_same
cmake --build out/is_same --target is_same_tests
./out/is_same/exercises/type_traits/is_same/is_same_tests
```
