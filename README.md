[English](README.md) | [Русский](README.ru.md)

# build-your-stl

Learn how the C++ standard library works by implementing its components yourself.
Exercises are independent and grouped by topic.

**[Browse exercises](EXERCISES.md)**

## Quick start

You need Linux or WSL, Git, a C++20 compiler, CMake 3.20 or later, and Make or Ninja.

Clone the repository and open its directory.

```sh
git clone https://github.com/Fleack/build-your-stl.git
cd build-your-stl
```

Start with the [`a + b` warm-up](exercises/warmup/add/README.md). Build and run its tests from the repository root. The tests will fail with the starter code.

```sh
cmake -S . -B out/add
cmake --build out/add --target add_tests
./out/add/exercises/warmup/add/add_tests
```

Implement `bys::add` in [`add.hpp`](exercises/warmup/add/include/bys/add.hpp), then rebuild and rerun the tests. When the summary shows `0 failed`, move on to [`bys::find`](exercises/algorithms/find/README.md).

## Documentation

- [Test framework](infrastructure/test/README.md)

Inspired by [build-your-own-x](https://github.com/codecrafters-io/build-your-own-x).

## License

This project is licensed under the [PolyForm Noncommercial License 1.0.0](LICENSE).
