[English](README.md) | [Русский](README.ru.md)

# build-your-stl

Изучайте устройство стандартной библиотеки C++, реализуя её компоненты самостоятельно.
Задания независимы и сгруппированы по темам.

**[Каталог заданий](EXERCISES.ru.md)**

## Быстрый старт

Нужны Linux или WSL, Git, компилятор с поддержкой C++20, CMake 3.20 или новее и Make или Ninja.

Клонируйте репозиторий и перейдите в его каталог.

```sh
git clone https://github.com/Fleack/build-your-stl.git
cd build-your-stl
```

Начните с [разминки `a + b`](exercises/warmup/add/README.ru.md). Соберите и запустите её тесты из корня репозитория. На заготовке тесты завершатся с ошибкой.

```sh
cmake -S . -B out/add
cmake --build out/add --target add_tests
./out/add/exercises/warmup/add/add_tests
```

Реализуйте `bys::add` в [`add.hpp`](exercises/warmup/add/include/bys/add.hpp), затем снова соберите и запустите тесты. Когда в итоговой строке появится `0 failed`, переходите к [`bys::find`](exercises/algorithms/find/README.ru.md).

## Документация

- [Тестовый фреймворк](infrastructure/test/README.ru.md)

Источник идеи — [build-your-own-x](https://github.com/codecrafters-io/build-your-own-x).

## Лицензия

Проект распространяется по лицензии [PolyForm Noncommercial License 1.0.0](LICENSE).
