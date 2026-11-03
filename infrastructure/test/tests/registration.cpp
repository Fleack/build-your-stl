#include <bys/test/test.hpp>

#include <algorithm>

TEST_CASE("registration.multiple_translation_units")
{
    const auto tests = bys::test::detail::registered_tests();
    CHECK(std::ranges::any_of(tests, [](const auto& test) { return test.name == "assertions.boolean_and_equal"; }));
    CHECK(std::ranges::any_of(tests, [](const auto& test) { return test.name == "exceptions.expected_types"; }));
    CHECK(std::ranges::any_of(tests, [](const auto& test) { return test.name == "registration.multiple_translation_units"; }));
    for (const auto& test : tests)
    {
        CHECK(test.function != nullptr);
        CHECK_FALSE(test.name.empty());
        CHECK(test.location.line() > 0);
    }
}
