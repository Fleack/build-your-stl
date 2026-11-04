[English](README.md) | [Русский](README.ru.md)

# `bys::equal`

Реализуйте поэлементное сравнение двух диапазонов в [include/bys/equal.hpp](include/bys/equal.hpp).

- [`std::equal` на cppreference](https://en.cppreference.com/cpp/algorithm/equal)
- [`std::equal` в проекте стандарта C++20](https://timsong-cpp.github.io/cppwp/n4861/alg.equal)

**Вход.** Четыре входных итератора `first1`, `last1`, `first2` и `last2`, задающие диапазоны `[first1, last1)` и `[first2, last2)`.

**Результат.** `true`, если диапазоны имеют одинаковую длину и все пары соответствующих элементов равны при сравнении через `==`. Иначе `false`.

## Сборка и запуск тестов

После реализации `bys::equal` выполните команды из корня проекта.

```sh
cmake -S . -B out/equal
cmake --build out/equal --target equal_tests
./out/equal/exercises/algorithms/equal/equal_tests
```
