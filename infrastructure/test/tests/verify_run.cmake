cmake_minimum_required(VERSION 3.20)

execute_process(
    COMMAND "${EXECUTABLE}" "${SCENARIO}" ${ARGUMENTS}
    RESULT_VARIABLE actual_exit
    OUTPUT_VARIABLE output
    ERROR_VARIABLE errors
)

if(NOT "${actual_exit}" STREQUAL "${EXPECTED_EXIT}")
    message(FATAL_ERROR "Expected exit ${EXPECTED_EXIT}, got ${actual_exit}\n${output}\n${errors}")
endif()

foreach(fragment IN LISTS EXPECTED_TEXT)
    string(FIND "${output}${errors}" "${fragment}" position)
    if(position EQUAL -1)
        message(FATAL_ERROR "Missing output fragment: ${fragment}\n${output}\n${errors}")
    endif()
endforeach()

if("${EXPECTED_EXIT}" STREQUAL "0" AND "--list" IN_LIST ARGUMENTS)
    if(NOT output STREQUAL "a.first\nz.after\n" OR NOT errors STREQUAL "")
        message(FATAL_ERROR "Listing must be sorted and must not execute tests\n${output}\n${errors}")
    endif()
endif()
