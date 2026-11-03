#include <ostream>

#include <bys/test/test.hpp>

namespace bys::test::detail
{
void report_test(std::ostream& output, TestCase const& test, TestContext const& context)
{
    output << (context.failures.empty() ? "[PASS] " : "[FAIL] ") << test.name << '\n';
    for (auto const& failure : context.failures)
    {
        output << "  " << failure.location.file_name() << ':' << failure.location.line() << '\n';
        output << "  " << failure.expression << '\n';
        if (!failure.message.empty())
        {
            output << "  " << failure.message << '\n';
        }
    }
}

void report_summary(std::ostream& output, RunSummary const& summary)
{
    output << "Tests: " << summary.passed_tests << " passed, " << summary.failed_tests << " failed\n";
    output << "Checks: " << summary.checks - summary.failed_checks << " passed, " << summary.failed_checks << " failed\n";
}
} // namespace bys::test::detail
