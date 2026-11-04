[English](README.md) | [Русский](README.ru.md)

# `bys::array`

Реализуйте контейнер фиксированного размера из C++20 в [include/bys/array.hpp](include/bys/array.hpp).

- [`std::array` на cppreference](https://en.cppreference.com/cpp/container/array)
- [`std::array` в проекте стандарта C++20](https://timsong-cpp.github.io/cppwp/n4861/array)

**Вход.** Тип элемента `T`, размер `N`, известный при компиляции и допускающий ноль, и до `N` инициализаторов.

**Результат.** Агрегат `bys::array<T, N>`, который хранит ровно `N` элементов в непрерывной области памяти. Поддержите типы и операции C++20 для размера, доступа к элементам, обхода, `fill` и `swap`. Добавьте вывод параметров шаблона, сравнения, свободные функции `bys::swap` и `bys::to_array`, а также интерфейс кортежа через `bys::get`, `std::tuple_size` и `std::tuple_element`.

## Сборка и запуск тестов

После реализации `bys::array` выполните команды из корня проекта.

```sh
cmake -S . -B out/array
cmake --build out/array --target array_tests
./out/array/exercises/containers/array/array_tests
```
