[English](README.md) | [Русский](README.ru.md)

# `bys::span`

Реализуйте `span` из C++20 в [include/bys/span.hpp](include/bys/span.hpp).

- [`std::span` на cppreference](https://en.cppreference.com/cpp/container/span)
- [`std::span` в проекте стандарта C++20](https://timsong-cpp.github.io/cppwp/n4861/views.span)
- [Исправление C++20 для представлений с фиксированным размером (P2325R3)](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p2325r3.html)

**Вход.** Тип элемента, фиксированный размер или `bys::dynamic_extent` и непрерывная последовательность. Её можно передать как массив, диапазон с известным размером, другой `span` или итератор с количеством элементов либо с ограничителем, от которого можно вычислить расстояние до итератора.

**Результат.** Представление, которое ссылается на исходные элементы без владения ими. Поддержите создание, вывод параметров шаблона, преобразования, доступ к элементам, обход, запросы размера и получение поддиапазонов по C++20. Поддержите концепты диапазонов `view` и `borrowed_range` и добавьте `bys::as_bytes` и `bys::as_writable_bytes`.

## Сборка и запуск тестов

После реализации `bys::span` выполните команды из корня проекта.

```sh
cmake -S . -B out/span
cmake --build out/span --target span_tests
./out/span/exercises/views/span/span_tests
```
