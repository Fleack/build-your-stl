[English](README.md) | [Русский](README.ru.md)

# `bys::is_same`

Реализуй `bys::is_same<T, U>` и `bys::is_same_v<T, U>` в [include/bys/is_same.hpp](include/bys/is_same.hpp).

Оба определяют на этапе компиляции, совпадают ли типы `T` и `U`. Псевдоним обозначает тот же тип. Различия в `const`, `volatile` и виде ссылки учитываются.

`is_same<T, U>` имеет интерфейс булевой константы. Результат доступен через `::value`, неявное преобразование в `bool` и вызов объекта.

## Сборка и запуск тестов

```sh
cmake -S . -B out/is_same
cmake --build out/is_same --target is_same_tests
./out/is_same/exercises/type_traits/is_same/is_same_tests
```
