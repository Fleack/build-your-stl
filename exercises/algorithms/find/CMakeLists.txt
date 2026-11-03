add_library(bys_find INTERFACE)
target_include_directories(bys_find INTERFACE include)
target_compile_features(bys_find INTERFACE cxx_std_20)

if(BUILD_TESTING)
    add_executable(find_tests tests/find.cpp)
    target_link_libraries(find_tests PRIVATE bys_find bys_test_main)
endif()
