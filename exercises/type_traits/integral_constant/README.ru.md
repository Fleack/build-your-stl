[English](README.md) | [Русский](README.ru.md)

# `bys::integral_constant`

Реализуй в [include/bys/integral_constant.hpp](include/bys/integral_constant.hpp) шаблон класса, который представляет значение времени компиляции в виде типа.

- [`std::integral_constant` на cppreference](https://en.cppreference.com/cpp/types/integral_constant)
- [`std::integral_constant` в проекте стандарта C++20](https://timsong-cpp.github.io/cppwp/n4861/meta.help)

Шаблон принимает тип `T` и значение `v` типа `T`. Поддержи любой `T`, допустимый для нетипового параметра шаблона в C++20.

## Сборка и запуск тестов

После реализации выполни команды из корня проекта.

```sh
cmake -S . -B out/integral_constant
cmake --build out/integral_constant --target integral_constant_tests
./out/integral_constant/exercises/type_traits/integral_constant/integral_constant_tests
```
