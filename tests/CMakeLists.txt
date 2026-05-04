include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.14.0
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)

function(croncpp_add_gtest target source)

endfunction()

add_executable(croncpp_tests
        cron_expression_tests.cpp
        croncpp_tests.cpp
)
set_target_properties(croncpp_tests PROPERTIES CXX_EXTENSIONS OFF)
target_compile_features(croncpp_tests PUBLIC ${CXX_STD_VER})
target_link_libraries(croncpp_tests PRIVATE croncpp croncpp_options GTest::gtest_main)
gtest_discover_tests(croncpp_tests)
