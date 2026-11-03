[English](README.md) | [Русский](README.ru.md)

# build-your-std

Learn how the C++ standard library works by implementing its components yourself.
Exercises are independent and grouped by topic.

**[Browse exercises](EXERCISES.md)**

## How it works

1. Choose an exercise.
2. Read its requirements and linked documentation.
3. Write your implementation.
4. Run the tests.
5. Compare performance when a benchmark is provided.

## Quick start

You need Linux or WSL, a C++20 compiler, CMake 3.20 or later, and a build tool such as Make or Ninja.
Run these commands from the project root to build and test the test framework:

```sh
cmake -S . -B out/Debug -DCMAKE_BUILD_TYPE=Debug
cmake --build out/Debug --parallel
ctest --test-dir out/Debug --output-on-failure
```

## Documentation

- [Exercise catalog](EXERCISES.md)
- [Test framework](infrastructure/test/README.md)

Inspired by [build-your-own-x](https://github.com/codecrafters-io/build-your-own-x).
