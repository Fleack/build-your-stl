[English](CONTRIBUTING.md) | [Русский](CONTRIBUTING.ru.md)

# Contributing

Use your personal fork to work through the exercises and keep your solutions there. Send pull requests to the main repository for fixes to exercises, documentation or translations, tests that cover missing behavior, improvements to the infrastructure, and new exercises.

## Open your first pull request

1. Fork [`Fleack/build-your-stl`](https://github.com/Fleack/build-your-stl) and clone your fork. Create a branch for your change, for example `git switch -c fix/find-tests`.
2. Make one focused change and [check it locally](#check-your-change).
3. Commit the change with a succinct and push your branch to your fork. Open a pull request from that branch to `Fleack/build-your-stl:master`.
4. Explain what changed, why, and how you checked it. For new tests, describe the behavior that the existing tests did not cover.

## Check your change

- For an exercise, build its test target and run it as described in the exercise README. Check new or changed tests with an implementation that meets the exercise requirements.
- For changes to the test framework or shared build configuration, run the [framework self-tests](infrastructure/test/README.md#check-the-framework).
- For documentation, check relative links and update both language versions together.

To run the same reference tests as CI, use a separate build directory.

```sh
cmake -S . -B out/reference -DBYS_REFERENCE_TESTS=ON
cmake --build out/reference --parallel
ctest --test-dir out/reference --output-on-failure
```

This mode tests the exercises against reference implementations and runs the framework self-tests. Use the exercise's own build instructions to test your implementation.

## Follow the code style

Format changed C++ files with [`.clang-format`](.clang-format). Use `snake_case` for functions, variables and file names, and `PascalCase` for helper types. Keep the standard library names for the APIs that exercises reproduce, such as `bys::vector` and `value_type`. Name test targets `<exercise>_tests` and test cases `<exercise>.<scenario>`.

## Add an exercise

- Keep the exercise independent and use C++20 and the project's test framework.
- Add README in English and if possible in Russian, a starter header and a separate CMake test target.
- Test the documented behavior and boundary cases. Add type, `constexpr` and complexity checks where the contract calls for them.
- Register the exercise and its standard library header in [`exercises/CMakeLists.txt`](exercises/CMakeLists.txt). List any additional standard library symbols that the tests use. Add a link to the [English catalog](EXERCISES.md) and the [Russian catalog](EXERCISES.ru.md).

## Review and merge

The repository owner reviews a PR after its required CI checks pass. To merge, the PR also needs an approving review from the owner, [@Fleack](https://github.com/Fleack), and must follow the code style and naming rules above.

CI checks C++ formatting with clang-format 22, C++ identifiers with clang-tidy 22, and links to repository files and Markdown headings. The test jobs run on Ubuntu 26.10 with GCC 16.2 and Clang 22.
