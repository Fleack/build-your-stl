[English](README.md) | [Русский](README.ru.md)

# `bys::upper_bound`

Реализуйте обе классические перегрузки `upper_bound` в [include/bys/upper_bound.hpp](include/bys/upper_bound.hpp).

- [`std::upper_bound` на cppreference](https://en.cppreference.com/cpp/algorithm/upper_bound)
- [`std::upper_bound` в проекте стандарта C++20](https://timsong-cpp.github.io/cppwp/n4861/upper.bound)

**Вход.** Два итератора прямого прохода `first` и `last`, искомое значение и, во второй перегрузке, компаратор `comp`. В диапазоне `[first, last)` все элементы, для которых `std::less<>{}(value, element)` или `comp(value, element)` ложно, стоят перед элементами, для которых сравнение истинно.

**Результат.** Итератор на первый элемент, для которого сравнение истинно, или `last`, если такого элемента нет.

## Сборка и запуск тестов

После реализации `bys::upper_bound` выполните команды из корня проекта.

```sh
cmake -S . -B out/upper_bound
cmake --build out/upper_bound --target upper_bound_tests
./out/upper_bound/exercises/algorithms/upper_bound/upper_bound_tests
```
