add_library(bys_equal INTERFACE)
target_include_directories(bys_equal INTERFACE include)
target_compile_features(bys_equal INTERFACE cxx_std_20)

if(BUILD_TESTING)
    add_executable(equal_tests tests/equal.cpp)
    target_link_libraries(equal_tests PRIVATE bys_equal bys_test_main)
endif()
