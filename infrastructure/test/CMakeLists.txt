add_library(bys_test STATIC
    src/registry.cpp
    src/runner.cpp
    src/report.cpp
)
target_include_directories(bys_test PUBLIC include)
target_compile_features(bys_test PUBLIC cxx_std_20)

add_library(bys_test_main STATIC src/main.cpp)
target_link_libraries(bys_test_main PUBLIC bys_test)

add_subdirectory(tests)
