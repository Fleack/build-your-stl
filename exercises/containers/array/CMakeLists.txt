add_library(bys_array INTERFACE)
target_include_directories(bys_array INTERFACE include)
target_compile_features(bys_array INTERFACE cxx_std_20)

if(BUILD_TESTING)
    add_executable(array_tests tests/array.cpp tests/tuple_and_creation.cpp)
    target_link_libraries(array_tests PRIVATE bys_array bys_test_main)
endif()
