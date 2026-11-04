add_library(bys_count INTERFACE)
target_include_directories(bys_count INTERFACE include)
target_compile_features(bys_count INTERFACE cxx_std_20)

if(BUILD_TESTING)
    add_executable(count_tests tests/count.cpp)
    target_link_libraries(count_tests PRIVATE bys_count bys_test_main)
endif()
