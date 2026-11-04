#include <sstream>
#include <stdexcept>
#include <thread>
#include <type_traits>

#include <bys/test/test.hpp>

namespace
{
struct Immovable
{
    int value;

    explicit Immovable(int initial)
        : value(initial)
    {}
    Immovable(Immovable const&) = delete;
    Immovable(Immovable&&) = delete;
    bool operator==(Immovable const&) const = default;
};

struct ThrowingPrinter
{
    int value;
    bool operator==(ThrowingPrinter const&) const = default;

    friend std::ostream& operator<<(std::ostream&, ThrowingPrinter const&) { throw std::runtime_error("printer failed"); }
};
} // namespace

TEST_CASE("assertions.boolean_and_equal")
{
    CHECK(true);
    CHECK_FALSE(false);
    REQUIRE(true);
    REQUIRE_FALSE(false);
    CHECK_EQ(12, 12);
    REQUIRE_EQ(std::string("value"), "value");
    CHECK(std::is_same_v<int, int>);
}

TEST_CASE("assertions.evaluate_once_in_order")
{
    int sequence = 0;
    auto const left = [&]() {
        CHECK_EQ(sequence, 0);
        ++sequence;
        return 7;
    };
    auto const right = [&]() {
        CHECK_EQ(sequence, 1);
        ++sequence;
        return 7;
    };
    CHECK_EQ(left(), right());
    CHECK_EQ(sequence, 2);

    int calls = 0;
    CHECK(++calls == 1);
    REQUIRE(++calls == 2);
    CHECK_FALSE(++calls == 0);
    REQUIRE_FALSE(++calls == 0);
    CHECK_EQ(calls, 4);
}

TEST_CASE("assertions.no_copy_or_move_required")
{
    Immovable left{3};
    Immovable right{3};
    CHECK_EQ(left, right);
    CHECK_EQ(Immovable{4}, Immovable{4});
}

TEST_CASE("assertions.failures_keep_location_and_expression")
{
    bys::test::detail::TestContext context;
    unsigned expected_line = 0;
    {
        bys::test::detail::ContextScope scope(context);
        expected_line = std::source_location::current().line() + 1;
        CHECK_EQ(1, 2);
        CHECK(false);
        CHECK(true);
    }
    REQUIRE_EQ(context.failures.size(), 2u);
    CHECK_EQ(context.checks, 3u);
    CHECK_EQ(context.failed_checks, 2u);
    CHECK_EQ(context.failures[0].location.line(), expected_line);
    CHECK(std::string_view(context.failures[0].location.file_name()).ends_with("assertions.cpp"));
    CHECK_EQ(context.failures[0].expression, "1 == 2");
    CHECK(context.failures[0].message.empty());
    CHECK_EQ(context.failures[1].expression, "false");
}

TEST_CASE("assertions.failed_comparisons_do_not_format_values")
{
    bys::test::detail::TestContext context;
    {
        bys::test::detail::ContextScope scope(context);
        CHECK_EQ(Immovable{1}, Immovable{2});
        CHECK_EQ(ThrowingPrinter{1}, ThrowingPrinter{2});
    }
    REQUIRE_EQ(context.failures.size(), 2u);
    CHECK(context.failures[0].message.empty());
    CHECK(context.failures[1].message.empty());
}

TEST_CASE("assertions.report_uses_check_source_location")
{
    bys::test::detail::TestContext context;
    std::vector<unsigned> lines;
    {
        bys::test::detail::ContextScope scope(context);
        lines.push_back(std::source_location::current().line() + 1);
        CHECK(false);
        lines.push_back(std::source_location::current().line() + 1);
        CHECK_EQ(1, 2);
        lines.push_back(std::source_location::current().line() + 1);
        CHECK_THROWS_AS(static_cast<void>(0), std::runtime_error);
        try
        {
            lines.push_back(std::source_location::current().line() + 1);
            REQUIRE_EQ(3, 4);
        }
        catch (bys::test::detail::AbortTest const&)
        {
        }
    }

    std::ostringstream output;
    auto const registration_location = std::source_location::current();
    bys::test::detail::report_test(output, {"location.example", +[]() {}, registration_location}, context);
    REQUIRE_EQ(context.failures.size(), lines.size());
    for (std::size_t index = 0; index < lines.size(); ++index)
    {
        CHECK_EQ(context.failures[index].location.line(), lines[index]);
    }
    std::string_view const path = " failed in: infrastructure/test/tests/assertions.cpp:";
    std::ostringstream expected;
    expected << "[FAIL] location.example\n\n"
             << "CHECK" << path << lines[0] << "\nfalse\n\n"
             << "CHECK" << path << lines[1] << "\n1 == 2\n\n"
             << "CHECK" << path << lines[2] << "\nstatic_cast<void>(0) throws std::runtime_error\nNo exception was thrown\n\n"
             << "REQUIRE" << path << lines[3] << "\n3 == 4\n\n";
    CHECK_EQ(output.str(), expected.str());
}

TEST_CASE("assertions.require_stops_and_unwinds")
{
    bool destroyed = false;
    bool continued = false;
    struct Guard
    {
        bool& destroyed;
        ~Guard() { destroyed = true; }
    };

    bys::test::detail::TestContext context;
    {
        bys::test::detail::ContextScope scope(context);
        try
        {
            Guard guard{destroyed};
            REQUIRE(false);
            continued = true;
        }
        catch (bys::test::detail::AbortTest const&)
        {
        }
    }
    CHECK(destroyed);
    CHECK_FALSE(continued);
    CHECK_EQ(context.failed_checks, 1u);
}

TEST_CASE("assertions.macros_are_single_statements")
{
    if (true)
    {
        CHECK(true);
    }
    else
    {
        REQUIRE(false);
    }
    if (true)
    {
        CHECK_EQ(1, 1);
    }
    else
    {
        REQUIRE_EQ(1, 2);
    }
}

TEST_CASE("assertions.worker_thread_has_no_context")
{
    bool rejected = false;
    std::thread worker([&]() {
        try
        {
            CHECK(true);
        }
        catch (std::logic_error const&)
        {
            rejected = true;
        }
    });
    worker.join();
    CHECK(rejected);
}
