#pragma once

#include <cstddef>
#include <exception>
#include <iosfwd>
#include <source_location>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace bys::test
{
struct TestCase
{
    std::string_view name;
    void (*function)();
    std::source_location location;
};

int run_tests(std::span<std::string_view const> arguments, std::ostream& output, std::ostream& errors);
int run_tests(int argc, char* argv[]);

namespace detail
{
enum class Severity
{
    check,
    require
};

struct AbortTest
{
};

struct Failure
{
    std::source_location location;
    std::string expression;
    std::string message;
};

struct TestContext
{
    std::size_t checks = 0;
    std::size_t failed_checks = 0;
    std::vector<Failure> failures;
};

struct RunSummary
{
    std::size_t passed_tests = 0;
    std::size_t failed_tests = 0;
    std::size_t checks = 0;
    std::size_t failed_checks = 0;
};

void register_test(TestCase test);
std::span<TestCase const> registered_tests();

struct Registrar
{
    Registrar(std::string_view name, void (*function)(), std::source_location location)
    {
        register_test({.name = name, .function = function, .location = location});
    }
};

TestContext& current_context();

class ContextScope
{
public:
    explicit ContextScope(TestContext& context);
    ~ContextScope();
    ContextScope(ContextScope const&) = delete;
    ContextScope& operator=(ContextScope const&) = delete;

private:
    TestContext* previous_;
};

void record_check(bool passed, std::string_view expression, std::string message, Severity severity, std::source_location location);
std::string describe_current_exception();
void report_test(std::ostream& output, TestCase const& test, TestContext const& context);
void report_summary(std::ostream& output, RunSummary const& summary);

inline void check(bool passed, std::string_view expression, Severity severity, std::source_location location)
{
    record_check(passed, expression, {}, severity, location);
}

template <class Left, class Right>
void check_equal(Left&& left, Right&& right, std::string_view expression, Severity severity, std::source_location location)
{
    check(static_cast<bool>(std::forward<Left>(left) == std::forward<Right>(right)), expression, severity, location);
}

template <class Exception, class Function>
void check_throws_as(Function&& function, std::string_view expression, Severity severity, std::source_location location)
{
    bool passed = false;
    std::string message;
    try
    {
        std::forward<Function>(function)();
        message = "No exception was thrown";
    }
    catch (AbortTest const&)
    {
        throw;
    }
    catch (...)
    {
        try
        {
            throw;
        }
        catch (Exception const&)
        {
            passed = true;
        }
        catch (...)
        {
            message = describe_current_exception();
        }
    }
    record_check(passed, expression, std::move(message), severity, location);
}
} // namespace detail
} // namespace bys::test

#define BYS_TEST_DETAIL_JOIN_IMPL(left, right) left##right
#define BYS_TEST_DETAIL_JOIN(left, right) BYS_TEST_DETAIL_JOIN_IMPL(left, right)

#define BYS_TEST_DETAIL_CASE(name, line)                                                                                                                                            \
    static void BYS_TEST_DETAIL_JOIN(bys_test_function_, line)();                                                                                                                   \
    namespace                                                                                                                                                                       \
    {                                                                                                                                                                               \
    const ::bys::test::detail::Registrar BYS_TEST_DETAIL_JOIN(bys_test_registrar_, line){name, &BYS_TEST_DETAIL_JOIN(bys_test_function_, line), ::std::source_location::current()}; \
    }                                                                                                                                                                               \
    static void BYS_TEST_DETAIL_JOIN(bys_test_function_, line)()

#define TEST_CASE(name) BYS_TEST_DETAIL_CASE(name, __LINE__)

#define BYS_TEST_DETAIL_CHECK(severity, ...)                                                                                     \
    do                                                                                                                           \
    {                                                                                                                            \
        ::bys::test::detail::check(static_cast<bool>((__VA_ARGS__)), #__VA_ARGS__, severity, ::std::source_location::current()); \
    }                                                                                                                            \
    while (false)

#define CHECK(...) BYS_TEST_DETAIL_CHECK(::bys::test::detail::Severity::check, __VA_ARGS__)
#define REQUIRE(...) BYS_TEST_DETAIL_CHECK(::bys::test::detail::Severity::require, __VA_ARGS__)
#define CHECK_FALSE(...) BYS_TEST_DETAIL_CHECK(::bys::test::detail::Severity::check, !(__VA_ARGS__))
#define REQUIRE_FALSE(...) BYS_TEST_DETAIL_CHECK(::bys::test::detail::Severity::require, !(__VA_ARGS__))

#define BYS_TEST_DETAIL_EQUAL(left, right, severity, line)                                                                                                     \
    do                                                                                                                                                         \
    {                                                                                                                                                          \
        auto&& BYS_TEST_DETAIL_JOIN(bys_test_left_, line) = (left);                                                                                            \
        auto&& BYS_TEST_DETAIL_JOIN(bys_test_right_, line) = (right);                                                                                          \
        ::bys::test::detail::check_equal(::std::forward<decltype(BYS_TEST_DETAIL_JOIN(bys_test_left_, line))>(BYS_TEST_DETAIL_JOIN(bys_test_left_, line)),     \
            ::std::forward<decltype(BYS_TEST_DETAIL_JOIN(bys_test_right_, line))>(BYS_TEST_DETAIL_JOIN(bys_test_right_, line)), #left " == " #right, severity, \
            ::std::source_location::current());                                                                                                                \
    }                                                                                                                                                          \
    while (false)

#define CHECK_EQ(left, right) BYS_TEST_DETAIL_EQUAL(left, right, ::bys::test::detail::Severity::check, __LINE__)
#define REQUIRE_EQ(left, right) BYS_TEST_DETAIL_EQUAL(left, right, ::bys::test::detail::Severity::require, __LINE__)

#define BYS_TEST_DETAIL_THROWS(expression, exception_type, severity)                                                                                                                         \
    do                                                                                                                                                                                       \
    {                                                                                                                                                                                        \
        ::bys::test::detail::check_throws_as<exception_type>([&]() { static_cast<void>(expression); }, #expression " throws " #exception_type, severity, ::std::source_location::current()); \
    }                                                                                                                                                                                        \
    while (false)

#define CHECK_THROWS_AS(expression, exception_type) BYS_TEST_DETAIL_THROWS(expression, exception_type, ::bys::test::detail::Severity::check)
#define REQUIRE_THROWS_AS(expression, exception_type) BYS_TEST_DETAIL_THROWS(expression, exception_type, ::bys::test::detail::Severity::require)
#define STATIC_CHECK(...) static_assert(__VA_ARGS__)
