[English](README.md) | [Русский](README.ru.md)

# Разминка: `a + b`

Реализуйте [`bys::add`](include/bys/add.hpp), чтобы функция возвращала сумму двух целых чисел.

Соберите и запустите тесты из корня репозитория. На заготовке тесты завершатся с ошибкой и пройдут после правильной реализации.

```sh
cmake -S . -B out/add
cmake --build out/add --target add_tests
./out/add/exercises/warmup/add/add_tests
```

Когда в итоговой строке тестов появится `0 failed`, переходите к [`bys::find`](../../algorithms/find/README.ru.md).
