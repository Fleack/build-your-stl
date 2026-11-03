[English](README.md) | [Русский](README.ru.md)

# `bys::find`

Реализуйте линейный поиск в [include/bys/find.hpp](include/bys/find.hpp).

- [`std::find` на cppreference](https://en.cppreference.com/cpp/algorithm/find)
- [`std::find` в проекте стандарта C++20](https://timsong-cpp.github.io/cppwp/n4861/alg.find)

**Вход.** Два входных итератора `first` и `last`, задающие диапазон `[first, last)`, и искомое значение.

**Результат.** Итератор на первый элемент, для которого `*iterator == value` истинно, или `last`, если совпадений нет.

## Сборка и запуск тестов

После реализации `bys::find` выполните команды из корня проекта.

```sh
cmake -S . -B out/find
cmake --build out/find --target find_tests
./out/find/exercises/algorithms/find/find_tests
```
