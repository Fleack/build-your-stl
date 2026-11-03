#include <bys/test/test.hpp>

#include <iostream>
#include <stdexcept>

namespace
{
void pass() { CHECK(true); }

void failed_check()
{
    CHECK(false);
    CHECK(true);
    std::cout << "check continued\n";
}

void failed_require()
{
    struct Guard
    {
        ~Guard() { std::cout << "guard destroyed\n"; }
    } guard;
    REQUIRE(false);
    throw std::logic_error("REQUIRE continued unexpectedly");
}

void unexpected_exception() { throw std::runtime_error("scenario exception"); }

void unknown_exception() { throw 42; }

void add(std::string_view name, void (*function)()) { bys::test::detail::register_test({name, function, std::source_location::current()}); }
} // namespace

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        return 3;
    }
    const std::string_view scenario = argv[1];
    if (scenario == "duplicate")
    {
        add("duplicate", pass);
        add("duplicate", pass);
    }
    else if (scenario == "null")
    {
        add("invalid", nullptr);
    }
    else if (scenario == "empty-name")
    {
        add("", pass);
    }
    else if (scenario != "empty")
    {
        add("z.after", pass);
        if (scenario == "pass")
            add("a.first", pass);
        else if (scenario == "check")
            add("a.first", failed_check);
        else if (scenario == "require")
            add("a.first", failed_require);
        else if (scenario == "exception")
            add("a.first", unexpected_exception);
        else if (scenario == "unknown-exception")
            add("a.first", unknown_exception);
        else
            return 3;
    }

    std::vector<std::string_view> arguments;
    for (int index = 2; index < argc; ++index)
    {
        arguments.emplace_back(argv[index]);
    }
    return bys::test::run_tests(arguments, std::cout, std::cerr);
}
