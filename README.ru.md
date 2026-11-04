[English](README.md) | [Русский](README.ru.md)

# build-your-stl

Изучайте устройство стандартной библиотеки C++, реализуя её компоненты самостоятельно.
Задания независимы и сгруппированы по темам.

**[Каталог заданий](EXERCISES.ru.md)**

## Как проходить задания

1. Выберите задание.
2. Прочитайте условие и документацию по ссылкам.
3. Напишите реализацию.
4. Запустите тесты.
5. Сравните производительность, если предусмотрен бенчмарк.

## Быстрый старт

Нужны Linux или WSL, компилятор C++20, CMake 3.20 или новее и средство сборки, например Make или Ninja.
Выполните команды из корня проекта для сборки и проверки тестового фреймворка:

```sh
cmake -S . -B out/Debug -DCMAKE_BUILD_TYPE=Debug
cmake --build out/Debug --parallel
ctest --test-dir out/Debug --output-on-failure
```

## Документация

- [Каталог заданий](EXERCISES.ru.md)
- [Тестовый фреймворк](infrastructure/test/README.ru.md)

Источник идеи — [build-your-own-x](https://github.com/codecrafters-io/build-your-own-x).
