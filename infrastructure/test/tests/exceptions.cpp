#include <bys/test/test.hpp>

#include <stdexcept>

TEST_CASE("exceptions.expected_types")
{
    int calls = 0;
    const auto throws = [&]() {
        ++calls;
        throw std::runtime_error("expected");
    };
    CHECK_THROWS_AS(throws(), std::runtime_error);
    REQUIRE_THROWS_AS(throws(), std::exception);
    CHECK_EQ(calls, 2);
    CHECK_THROWS_AS(throw 42, int);
}

TEST_CASE("exceptions.mismatch_and_missing_exception")
{
    bys::test::detail::TestContext context;
    bool continued = false;
    {
        bys::test::detail::ContextScope scope(context);
        CHECK_THROWS_AS(static_cast<void>(0), std::runtime_error);
        CHECK_THROWS_AS(throw std::logic_error("different"), std::runtime_error);
        CHECK_THROWS_AS(throw 42, std::runtime_error);
        continued = true;
    }
    CHECK(continued);
    REQUIRE_EQ(context.failures.size(), 3u);
    CHECK_EQ(context.failed_checks, 3u);
    CHECK_EQ(context.failures[0].message, "No exception was thrown");
    CHECK_EQ(context.failures[1].message, "Unexpected exception: different");
    CHECK_EQ(context.failures[2].message, "Unexpected exception of a different type");
}

TEST_CASE("exceptions.require_throws_stops")
{
    bys::test::detail::TestContext context;
    bool continued = false;
    {
        bys::test::detail::ContextScope scope(context);
        try
        {
            REQUIRE_THROWS_AS(static_cast<void>(0), int);
            continued = true;
        }
        catch (const bys::test::detail::AbortTest&)
        {
        }
    }
    CHECK_FALSE(continued);
    CHECK_EQ(context.failed_checks, 1u);
}

TEST_CASE("exceptions.internal_abort_is_not_expected_exception")
{
    bys::test::detail::TestContext context;
    bool aborted = false;
    {
        bys::test::detail::ContextScope scope(context);
        try
        {
            CHECK_THROWS_AS([]() { REQUIRE(false); }(), bys::test::detail::AbortTest);
        }
        catch (const bys::test::detail::AbortTest&)
        {
            aborted = true;
        }
    }
    CHECK(aborted);
    CHECK_EQ(context.checks, 1u);
    CHECK_EQ(context.failed_checks, 1u);
}
