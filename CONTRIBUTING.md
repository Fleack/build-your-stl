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

## Follow the code style

Format changed C++ files with [`.clang-format`](.clang-format). Use `snake_case` for functions, variables and file names, and `PascalCase` for helper types. Keep the standard library names for the APIs that exercises reproduce, such as `bys::vector` and `value_type`. Name test targets `<exercise>_tests` and test cases `<exercise>.<scenario>`.

## Add an exercise

- Keep the exercise independent and use C++20 and the project's test framework.
- Add README in English and if possible in Russian, a starter header and a separate CMake test target.
- Test the documented behavior and boundary cases. Add type, `constexpr` and complexity checks where the contract calls for them.
- Register the exercise in [`exercises/CMakeLists.txt`](exercises/CMakeLists.txt) and add a link to the [English catalog](EXERCISES.md) and the [Russian catalog](EXERCISES.ru.md).

## Review and merge

The repository owner reviews a PR after its required CI checks pass. To merge, the PR also needs an approving review from the owner, [@Fleack](https://github.com/Fleack), and must follow the code style and naming rules above.
