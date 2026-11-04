add_library(bys_span INTERFACE)
target_include_directories(bys_span INTERFACE include)
target_compile_features(bys_span INTERFACE cxx_std_20)

if(BUILD_TESTING)
    add_executable(span_tests tests/construction.cpp tests/subviews.cpp)
    target_link_libraries(span_tests PRIVATE bys_span bys_test_main)
endif()
