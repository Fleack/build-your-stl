cmake_minimum_required(VERSION 3.20)

execute_process(
    COMMAND "${COMPILER}" -std=c++20 "-I${INCLUDE_DIR}" -fsyntax-only "${SOURCE}"
    RESULT_VARIABLE actual_exit
    OUTPUT_VARIABLE output
    ERROR_VARIABLE errors
)

if("${actual_exit}" STREQUAL "0")
    message(FATAL_ERROR "STATIC_CHECK(false, message) must reject compilation")
endif()
string(FIND "${output}${errors}" "STATIC_CHECK_CUSTOM_DIAGNOSTIC" position)
if(position EQUAL -1)
    message(FATAL_ERROR "Compilation failed without the custom assertion message\n${output}\n${errors}")
endif()
