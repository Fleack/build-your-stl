#include <stdexcept>

#include <bys/test/test.hpp>

namespace bys::test::detail
{
namespace
{
std::vector<TestCase>& registry()
{
    static std::vector<TestCase> tests;
    return tests;
}

thread_local TestContext* active_context = nullptr;
} // namespace

void register_test(TestCase test) { registry().push_back(test); }

std::span<TestCase const> registered_tests() { return registry(); }

TestContext& current_context()
{
    if (active_context == nullptr)
    {
        throw std::logic_error("A test assertion was used outside the executing test thread");
    }
    return *active_context;
}

ContextScope::ContextScope(TestContext& context)
    : previous_(active_context)
{ active_context = &context; }

ContextScope::~ContextScope() { active_context = previous_; }

void record_check(bool passed, std::string_view expression, std::string message, Severity severity, std::source_location location)
{
    auto& context = current_context();
    ++context.checks;
    if (passed)
    {
        return;
    }

    ++context.failed_checks;
    context.failures.push_back({location, std::string(expression), std::move(message)});
    if (severity == Severity::require)
    {
        throw AbortTest{};
    }
}

std::string describe_current_exception()
{
    try
    {
        throw;
    }
    catch (std::exception const& error)
    {
        return "Unexpected exception: " + std::string(error.what());
    }
    catch (...)
    {
        return "Unexpected exception of a different type";
    }
}
} // namespace bys::test::detail
