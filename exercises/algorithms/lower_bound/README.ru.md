[English](README.md) | [Русский](README.ru.md)

# `bys::lower_bound`

Реализуйте обе классические перегрузки `lower_bound` в [include/bys/lower_bound.hpp](include/bys/lower_bound.hpp).

- [`std::lower_bound` на cppreference](https://en.cppreference.com/cpp/algorithm/lower_bound)
- [`std::lower_bound` в проекте стандарта C++20](https://timsong-cpp.github.io/cppwp/n4861/lower.bound)

**Вход.** Два итератора прямого прохода `first` и `last`, искомое значение и, во второй перегрузке, компаратор `comp`. Диапазон `[first, last)` разбит по результату `std::less<>{}(element, value)` или `comp(element, value)`: все элементы, для которых сравнение истинно, стоят перед элементами, для которых оно ложно.

**Результат.** Итератор на первый элемент, для которого сравнение ложно, или `last`, если оно истинно для всех элементов.

## Сборка и запуск тестов

После реализации `bys::lower_bound` выполните команды из корня проекта.

```sh
cmake -S . -B out/lower_bound
cmake --build out/lower_bound --target lower_bound_tests
./out/lower_bound/exercises/algorithms/lower_bound/lower_bound_tests
```
