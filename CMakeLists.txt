cmake_minimum_required(VERSION 3.28)
project(croncpp VERSION 0.0.0 LANGUAGES CXX)

if(NOT DEFINED CXX_STD_VER)
  set(CXX_STD_VER cxx_std_17)
endif()

if (MSVC)
    add_compile_options(/W4)
else ()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror -Wconversion)
endif ()

add_library(croncpp INTERFACE)
target_include_directories(
  croncpp INTERFACE $<INSTALL_INTERFACE:include>
                    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_compile_features(croncpp INTERFACE ${CXX_STD_VER})
add_library(croncpp::croncpp ALIAS croncpp)

install(
  TARGETS croncpp
  EXPORT croncpp_targets
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
)

install(DIRECTORY include/ DESTINATION include)
include(CMakePackageConfigHelpers)
write_basic_package_version_file(croncppConfig.cmake COMPATIBILITY AnyNewerVersion)

install(
  EXPORT croncpp_targets
  FILE croncppConfig.cmake
  NAMESPACE croncpp::
  DESTINATION lib/cmake/croncpp)

add_library(croncpp_options INTERFACE)
target_compile_features(croncpp_options INTERFACE ${CXX_STD_VER})

if(WIN32)
  target_compile_options(croncpp_options INTERFACE "/EHc")
  target_compile_options(croncpp_options INTERFACE "/Zc:__cplusplus")
  target_compile_definitions(croncpp_options
                             INTERFACE "-D_SCL_SECURE_NO_WARNINGS")
elseif(APPLE)
  target_compile_options(croncpp_options INTERFACE -fexceptions -g -Wall)
else()
  target_compile_options(croncpp_options INTERFACE -fexceptions -g -Wall)
  # enable profiling
  if(CRONCPP_ENABLE_PROFILING)
    target_compile_options(
      croncpp_options
      INTERFACE --coverage -g -O0 -fprofile-arcs -ftest-coverage)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage -lgcov" CACHE STRING "Linker Flags for Profiling" FORCE)
  endif()
endif()

option(CRONCPP_BUILD_TESTS "Build the croncpp tests" ON)
option(CRONCPP_BUILD_EXAMPLE "Build the croncpp example executable" ON)

if(CRONCPP_BUILD_EXAMPLE)
  add_executable(croncpp_example main.cpp)
  set_target_properties(croncpp_example PROPERTIES CXX_EXTENSIONS OFF)
  target_link_libraries(croncpp_example PRIVATE croncpp croncpp_options)
endif()

if(CRONCPP_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
