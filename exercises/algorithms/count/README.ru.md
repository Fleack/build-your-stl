[English](README.md) | [Русский](README.ru.md)

# `bys::count`

Реализуйте подсчёт совпадений в [include/bys/count.hpp](include/bys/count.hpp).

- [`std::count` на cppreference](https://en.cppreference.com/cpp/algorithm/count)
- [`std::count` в проекте стандарта C++20](https://timsong-cpp.github.io/cppwp/n4861/alg.count)

**Вход.** Два входных итератора `first` и `last`, задающие диапазон `[first, last)`, и значение для сравнения.

**Результат.** Число элементов, для которых `*iterator == value` истинно. Тип результата — `difference_type` итератора.

## Сборка и запуск тестов

После реализации `bys::count` выполните команды из корня проекта.

```sh
cmake -S . -B out/count
cmake --build out/count --target count_tests
./out/count/exercises/algorithms/count/count_tests
```
