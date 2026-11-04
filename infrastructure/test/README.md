[English](README.md) | [Русский](README.ru.md)

# Test framework

A small C++20 test framework for build-your-stl.

## Add a test target

Add this target to the project's root `CMakeLists.txt`:

```cmake
add_executable(example_tests tests/example.cpp)
target_link_libraries(example_tests PRIVATE bys_test_main)
add_test(NAME example COMMAND example_tests)
```

Add test source files directly to the executable so that all tests are included.

For a custom `main()`, link `bys_test` instead of `bys_test_main` and use:

```cpp
#include <bys/test/test.hpp>

int main(int argc, char* argv[])
{
    return bys::test::run_tests(argc, argv);
}
```

## Write a test

Save this as `tests/example.cpp`:

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

## Assertions

`CHECK` records a failure and continues the test. `REQUIRE` records a failure and stops the current test.

| Macro | Checks |
| --- | --- |
| `TEST_CASE("name")` | Declares a test |
| `CHECK(expression)` / `REQUIRE(expression)` | The expression is true |
| `CHECK_FALSE(expression)` / `REQUIRE_FALSE(expression)` | The expression is false |
| `CHECK_EQ(left, right)` / `REQUIRE_EQ(left, right)` | The values compare equal |
| `CHECK_THROWS_AS(expression, Type)` / `REQUIRE_THROWS_AS(expression, Type)` | The expression throws an exception of the given type or a derived type |
| `STATIC_CHECK(expression, "message")` | The expression is true at compile time; otherwise compilation fails with the message |

## Run tests

After adding the example target, build and run it from the project root.

```sh
cmake -S . -B out/Debug -DCMAKE_BUILD_TYPE=Debug
cmake --build out/Debug --target example_tests
./out/Debug/example_tests
./out/Debug/example_tests --list
./out/Debug/example_tests --filter vector.
./out/Debug/example_tests --run vector.access
./out/Debug/example_tests --help
```

`--filter` selects names containing the given text. `--run` selects an exact name.
Use one selection option at a time.

## Read the result

Changing the expected value in `CHECK_EQ` to `0` produces:

```text
[FAIL] vector.access

CHECK failed in: tests/example.cpp:14
values.front() == 0

Tests: 0 passed, 1 failed
Checks: 2 passed, 1 failed

TESTS FAILED
```

Each failure shows its expression and the file and line, with project files relative to the project root.
In a Linux or WSL terminal, `ALL TESTS PASSED` appears in green and `TESTS FAILED` appears in red.

| Exit code | Meaning |
| --- | --- |
| `0` | All selected tests passed, or the test list or help was shown |
| `1` | A test failed |
| `2` | Invalid arguments, no matching tests, or invalid test names |

## Check the framework

Run the framework's own tests from the project root.

```sh
cmake -S . -B out/Debug -DCMAKE_BUILD_TYPE=Debug
cmake --build out/Debug --parallel
ctest --test-dir out/Debug --output-on-failure
```

[Back to the project](../../README.md)
