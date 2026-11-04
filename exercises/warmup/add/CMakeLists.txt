add_library(bys_add INTERFACE)
target_include_directories(bys_add INTERFACE include)
target_compile_features(bys_add INTERFACE cxx_std_20)

if(BUILD_TESTING)
    add_executable(add_tests tests/add.cpp)
    target_link_libraries(add_tests PRIVATE bys_add bys_test_main)
endif()
