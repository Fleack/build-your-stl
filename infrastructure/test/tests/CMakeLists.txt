find_package(Threads REQUIRED)

add_executable(bys_test_tests
    assertions.cpp
    registration.cpp
    exceptions.cpp
    compile_time.cpp
)
target_link_libraries(bys_test_tests PRIVATE bys_test_main Threads::Threads)
add_test(NAME framework.unit COMMAND bys_test_tests)

add_executable(bys_test_scenarios failure_scenarios.cpp)
target_link_libraries(bys_test_scenarios PRIVATE bys_test)

function(add_runner_case name scenario expected_exit expected_text)
    add_test(NAME framework.${name}
        COMMAND ${CMAKE_COMMAND}
            "-DEXECUTABLE=$<TARGET_FILE:bys_test_scenarios>"
            "-DSCENARIO=${scenario}"
            "-DEXPECTED_EXIT=${expected_exit}"
            "-DEXPECTED_TEXT=${expected_text}"
            "-DARGUMENTS=${ARGN}"
            -P ${CMAKE_CURRENT_SOURCE_DIR}/verify_run.cmake
    )
endfunction()

add_runner_case(pass pass 0 "Tests: 2 passed, 0 failed;Checks: 2 passed, 0 failed")
add_runner_case(check_continues check 1 "check continued;[PASS] z.after;Tests: 1 passed, 1 failed;Checks: 2 passed, 1 failed")
add_runner_case(require_unwinds require 1 "guard destroyed;[PASS] z.after;Tests: 1 passed, 1 failed;Checks: 1 passed, 1 failed")
add_runner_case(unexpected_exception exception 1 "Unexpected exception;scenario exception;[PASS] z.after;Checks: 1 passed, 0 failed")
add_runner_case(unknown_exception unknown-exception 1 "Non-standard exception;[PASS] z.after")
add_runner_case(duplicate duplicate 2 "duplicate test name")
add_runner_case(null_function null 2 "invalid test registration")
add_runner_case(empty_name empty-name 2 "invalid test registration")
add_runner_case(empty_registry empty 2 "no tests selected")
add_runner_case(list pass 0 "a.first\nz.after" --list)
add_runner_case(list_does_not_execute require 0 "a.first\nz.after" --list)
add_runner_case(filter pass 0 "[PASS] z.after;Tests: 1 passed, 0 failed" --filter after)
add_runner_case(exact pass 0 "[PASS] a.first;Tests: 1 passed, 0 failed" --run a.first)
add_runner_case(no_matches pass 2 "no tests selected" --filter missing)
add_runner_case(exact_is_not_substring pass 2 "no tests selected" --run first)
add_runner_case(unknown_argument pass 2 "unknown, repeated, or conflicting argument" --invalid)
add_runner_case(missing_value pass 2 "requires a non-empty value" --run)
add_runner_case(empty_value pass 2 "requires a non-empty value" --run --list)
add_runner_case(conflicting_selection pass 2 "unknown, repeated, or conflicting argument" --run a.first --filter first)
add_runner_case(repeated_argument pass 2 "unknown, repeated, or conflicting argument" --list --list)
add_runner_case(help empty 0 "Usage:" --help)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_test(NAME framework.static_check_rejects_false
        COMMAND ${CMAKE_COMMAND}
            "-DCOMPILER=${CMAKE_CXX_COMPILER}"
            "-DINCLUDE_DIR=${PROJECT_SOURCE_DIR}/infrastructure/test/include"
            "-DSOURCE=${CMAKE_CURRENT_SOURCE_DIR}/compile_fail.cpp"
            -P ${CMAKE_CURRENT_SOURCE_DIR}/verify_compile_fail.cmake
    )
endif()
