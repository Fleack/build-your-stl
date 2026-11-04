add_library(bys_lower_bound INTERFACE)
target_include_directories(bys_lower_bound INTERFACE include)
target_compile_features(bys_lower_bound INTERFACE cxx_std_20)

if(BUILD_TESTING)
    add_executable(lower_bound_tests tests/lower_bound.cpp)
    target_link_libraries(lower_bound_tests PRIVATE bys_lower_bound bys_test_main)
endif()
