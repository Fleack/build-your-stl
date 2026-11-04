#include <filesystem>
#include <iostream>

#if defined(__unix__) || defined(__APPLE__)
# include <unistd.h>
#endif

#include <bys/test/test.hpp>

namespace bys::test::detail
{
namespace
{
std::string display_path(char const* file_name)
{
    static std::filesystem::path const project_root = BYS_TEST_PROJECT_ROOT;
    auto const relative = std::filesystem::path(file_name).lexically_normal().lexically_relative(project_root);
    if (relative.empty() || *relative.begin() == "..")
    {
        return file_name;
    }
    return relative.generic_string();
}
} // namespace

void report_test(std::ostream& output, TestCase const& test, TestContext const& context)
{
    if (context.failures.empty())
    {
        output << "[PASS] " << test.name << '\n';
        return;
    }

    output << "[FAIL] " << test.name << "\n\n";
    for (auto const& failure : context.failures)
    {
        std::string_view label = "TEST";
        if (failure.severity)
        {
            label = *failure.severity == Severity::require ? "REQUIRE" : "CHECK";
        }
        output << label << " failed in: " << display_path(failure.location.file_name()) << ':' << failure.location.line() << '\n';
        output << failure.expression << '\n';
        if (!failure.message.empty())
        {
            output << failure.message << '\n';
        }
        output << '\n';
    }
}

void report_summary(std::ostream& output, RunSummary const& summary)
{
    output << "Tests: " << summary.passed_tests << " passed, " << summary.failed_tests << " failed\n";
    output << "Checks: " << summary.checks - summary.failed_checks << " passed, " << summary.failed_checks << " failed\n";

    bool const passed = summary.failed_tests == 0;
    bool const use_color =
#if defined(__unix__) || defined(__APPLE__)
        &output == &std::cout && ::isatty(STDOUT_FILENO);
#else
        false;
#endif
    output << '\n';
    if (use_color)
    {
        output << (passed ? "\x1b[32m" : "\x1b[31m");
    }
    output << (passed ? "ALL TESTS PASSED" : "TESTS FAILED");
    if (use_color)
    {
        output << "\x1b[0m";
    }
    output << '\n';
}
} // namespace bys::test::detail
