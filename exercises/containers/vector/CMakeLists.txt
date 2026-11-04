add_library(bys_vector INTERFACE)
target_include_directories(bys_vector INTERFACE include)
target_compile_features(bys_vector INTERFACE cxx_std_20)

if(BUILD_TESTING)
    add_executable(vector_tests
        tests/vector.cpp
        tests/modifiers.cpp
        tests/lifetime.cpp
        tests/allocator.cpp
    )
    target_link_libraries(vector_tests PRIVATE bys_vector bys_test_main)
endif()
