#include <algorithm>
#include <iostream>
#include <optional>

#include <bys/test/test.hpp>

namespace bys::test
{
namespace
{
constexpr std::string_view usage = "Usage: <test-executable> [--list] [--filter <substring> | --run <exact-name>]\n"
                                   "       <test-executable> --help\n";

struct Options
{
    bool list = false;
    bool help = false;
    std::optional<std::string_view> filter;
    std::optional<std::string_view> exact_name;
};

using TestList = std::vector<TestCase const*>;

std::optional<Options> parse_arguments(std::span<std::string_view const> arguments, std::ostream& errors)
{
    Options options;
    for (std::size_t index = 0; index < arguments.size(); ++index)
    {
        auto const argument = arguments[index];
        if (argument == "--list" && !options.list)
        {
            options.list = true;
        }
        else if (argument == "--help" && arguments.size() == 1)
        {
            options.help = true;
        }
        else if ((argument == "--filter" || argument == "--run") && !options.filter && !options.exact_name)
        {
            if (++index == arguments.size() || arguments[index].empty() || arguments[index].starts_with("--"))
            {
                errors << "Error: " << argument << " requires a non-empty value\n";
                return std::nullopt;
            }
            if (argument == "--filter")
            {
                options.filter = arguments[index];
            }
            else
            {
                options.exact_name = arguments[index];
            }
        }
        else
        {
            errors << "Error: unknown, repeated, or conflicting argument: " << argument << '\n';
            return std::nullopt;
        }
    }
    return options;
}

std::optional<TestList> collect_tests(std::ostream& errors)
{
    TestList tests;
    for (auto const& test : detail::registered_tests())
    {
        if (test.name.empty() || test.function == nullptr)
        {
            errors << "Error: invalid test registration at " << test.location.file_name() << ':' << test.location.line() << '\n';
            return std::nullopt;
        }
        tests.push_back(&test);
    }
    std::ranges::sort(tests, [](TestCase const* left, TestCase const* right) { return left->name < right->name; });
    if (auto duplicate = std::ranges::adjacent_find(tests, [](TestCase const* left, TestCase const* right) { return left->name == right->name; }); duplicate != tests.end())
    {
        errors << "Error: duplicate test name: " << (*duplicate)->name << '\n';
        return std::nullopt;
    }
    return tests;
}

bool select_tests(TestList& tests, Options const& options, std::ostream& errors)
{
    std::erase_if(
        tests, [&](TestCase const* test) {
            return (options.filter && test->name.find(*options.filter) == std::string_view::npos) ||
                (options.exact_name && test->name != *options.exact_name);
        });
    if (tests.empty())
    {
        errors << "Error: no tests selected\n";
        return false;
    }
    return true;
}

void list_tests(TestList const& tests, std::ostream& output)
{
    for (auto const* test : tests)
    {
        output << test->name << '\n';
    }
}

detail::TestContext execute_test(TestCase const& test)
{
    detail::TestContext context;
    detail::ContextScope scope(context);
    try
    {
        test.function();
    }
    catch (detail::AbortTest const&)
    {
        if (context.failures.empty())
        {
            context.failures.push_back({test.location, "Unexpected test abort", {}});
        }
    }
    catch (std::exception const& error)
    {
        context.failures.push_back({test.location, "Unexpected exception", error.what()});
    }
    catch (...)
    {
        context.failures.push_back({test.location, "Unexpected exception", "Non-standard exception"});
    }
    return context;
}

detail::RunSummary execute_tests(TestList const& tests, std::ostream& output)
{
    detail::RunSummary summary;
    for (auto const* test : tests)
    {
        auto const context = execute_test(*test);
        if (context.failures.empty())
        {
            ++summary.passed_tests;
        }
        else
        {
            ++summary.failed_tests;
        }
        summary.checks += context.checks;
        summary.failed_checks += context.failed_checks;
        detail::report_test(output, *test, context);
    }
    return summary;
}
} // namespace

int run_tests(std::span<std::string_view const> arguments, std::ostream& output, std::ostream& errors)
{
    auto const options = parse_arguments(arguments, errors);
    if (!options)
    {
        errors << usage;
        return 2;
    }
    if (options->help)
    {
        output << usage;
        return 0;
    }

    auto tests = collect_tests(errors);
    if (!tests || !select_tests(*tests, *options, errors))
    {
        return 2;
    }
    if (options->list)
    {
        list_tests(*tests, output);
        return 0;
    }

    auto const summary = execute_tests(*tests, output);
    detail::report_summary(output, summary);
    return summary.failed_tests == 0 ? 0 : 1;
}

int run_tests(int argc, char* argv[])
{
    std::vector<std::string_view> arguments;
    arguments.reserve(argc - 1);
    for (int index = 1; index < argc; ++index)
    {
        arguments.emplace_back(argv[index]);
    }
    return run_tests(arguments, std::cout, std::cerr);
}
} // namespace bys::test
