#include <algorithm>

#include <bys/test/test.hpp>

TEST_CASE("registration.multiple_translation_units")
{
    auto const tests = bys::test::detail::registered_tests();
    CHECK(std::ranges::any_of(tests, [](auto const& test) { return test.name == "assertions.boolean_and_equal"; }));
    CHECK(std::ranges::any_of(tests, [](auto const& test) { return test.name == "exceptions.expected_types"; }));
    CHECK(std::ranges::any_of(tests, [](auto const& test) { return test.name == "registration.multiple_translation_units"; }));
    for (auto const& test : tests)
    {
        CHECK(test.function != nullptr);
        CHECK_FALSE(test.name.empty());
        CHECK(test.location.line() > 0);
    }
}
