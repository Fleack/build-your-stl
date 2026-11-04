[English](README.md) | [Русский](README.ru.md)

# Тестовый фреймворк

Небольшой тестовый фреймворк на C++20 для build-your-stl.

## Подключение

Добавьте цель в корневой `CMakeLists.txt` проекта:

```cmake
add_executable(example_tests tests/example.cpp)
target_link_libraries(example_tests PRIVATE bys_test_main)
add_test(NAME example COMMAND example_tests)
```

Добавляйте исходники тестов непосредственно в исполняемый файл, чтобы все тесты вошли в сборку.

Для собственного `main()` подключите `bys_test` вместо `bys_test_main` и используйте:

```cpp
#include <bys/test/test.hpp>

int main(int argc, char* argv[])
{
    return bys::test::run_tests(argc, argv);
}
```

## Первый тест

Сохраните пример в `tests/example.cpp`:

```cpp
#include <bys/test/test.hpp>

#include <stdexcept>
#include <type_traits>
#include <vector>

STATIC_CHECK(std::is_same_v<std::vector<int>::value_type, int>, "value_type must be int");

TEST_CASE("vector.access")
{
    std::vector<int> values{42};

    REQUIRE(!values.empty());
    CHECK_EQ(values.front(), 42);
    CHECK_THROWS_AS(values.at(1), std::out_of_range);
}
```

## Проверки

`CHECK` записывает ошибку и продолжает тест. `REQUIRE` записывает ошибку и прекращает текущий тест.

| Макрос | Что проверяет |
| --- | --- |
| `TEST_CASE("name")` | Объявляет тест |
| `CHECK(expression)` / `REQUIRE(expression)` | Выражение истинно |
| `CHECK_FALSE(expression)` / `REQUIRE_FALSE(expression)` | Выражение ложно |
| `CHECK_EQ(left, right)` / `REQUIRE_EQ(left, right)` | Значения равны |
| `CHECK_THROWS_AS(expression, Type)` / `REQUIRE_THROWS_AS(expression, Type)` | Выражение выбрасывает исключение указанного типа или производного от него |
| `STATIC_CHECK(expression, "message")` | Выражение истинно во время компиляции; иначе сборка останавливается с сообщением |

## Запуск

После подключения примера соберите и запустите его из корня проекта.

```sh
cmake -S . -B out/Debug -DCMAKE_BUILD_TYPE=Debug
cmake --build out/Debug --target example_tests
./out/Debug/example_tests
./out/Debug/example_tests --list
./out/Debug/example_tests --filter vector.
./out/Debug/example_tests --run vector.access
./out/Debug/example_tests --help
```

`--filter` выбирает имена, содержащие указанный текст. `--run` выбирает точное имя.
Используйте только один параметр выбора за раз.

## Результат

Если заменить ожидаемое значение в `CHECK_EQ` на `0`, получим:

```text
[FAIL] vector.access

CHECK failed in: tests/example.cpp:14
values.front() == 0

Tests: 0 passed, 1 failed
Checks: 2 passed, 1 failed

TESTS FAILED
```

Для каждой проваленной проверки отчёт указывает выражение, путь к файлу и номер строки. Пути к файлам проекта отсчитываются от его корня.
В терминале Linux или WSL строка `ALL TESTS PASSED` выводится зелёным, а `TESTS FAILED` — красным.

| Код завершения | Значение |
| --- | --- |
| `0` | Все выбранные тесты прошли либо выведены список тестов или справка |
| `1` | Тест завершился с ошибкой |
| `2` | Неверные параметры, нет подходящих тестов или недопустимые имена тестов |

## Самопроверка фреймворка

Запустите собственные тесты фреймворка из корня проекта.

```sh
cmake -S . -B out/Debug -DCMAKE_BUILD_TYPE=Debug
cmake --build out/Debug --parallel
ctest --test-dir out/Debug --output-on-failure
```

[Вернуться к проекту](../../README.ru.md)
