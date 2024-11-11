# 指定 cmake 最小版本
cmake_minimum_required(VERSION 3.5)

# 指定项目名称
project(cpputils
  VERSION 0.0.1
  LANGUAGES CXX C ASM)

# 指定适用的操作系统
if(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
  message(FATAL_ERROR "This project requires Linux operating system.")
endif()

# 设置编译提醒
if(NOT CMAKE_BUILD_TYPE AND EXISTS "${CMAKE_SOURCE_DIR}/.git")
  set(default_build_type "Debug")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Default BUILD_TYPE is Debug, other options are: RelWithDebInfo, Release, and MinSizeRel." FORCE)
endif()

# 设置 C++ 标准为 C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# we use `asm()` to inline assembly, so enable the GNU extension
set(CMAKE_C_EXTENSIONS ON)
set(C_STANDARD_REQUIRED ON)

# 指定成果物路径
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules/")

# 指定文件搜索路径
# include_directories(${CMAKE_SOURCE_DIR}/include)

add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-fpermissive>)

include(DependencyChecks)

set(common_srcs
  common/dout.cc
  common/backtrace.cc
  common/assertion.cc
  common/formatter.cc)

# 创建动态库
add_library(common SHARED ${common_srcs})
target_link_libraries(common ${target_deps})


# 测试
# Used by googletest.
check_cxx_compiler_flag(-Wno-missing-field-initializers
                        CPPUTILS_HAVE_NO_MISSING_FIELD_INITIALIZERS)
check_cxx_compiler_flag(-Wno-implicit-int-float-conversion
                        CPPUTILS_HAVE_NO_IMPLICIT_INT_FLOAT_CONVERSION)

option(CPPUTILS_BUILD_TESTS "Build cpputils's own tests." ON)
if(CPPUTILS_BUILD_TESTS)

  enable_testing()

  # This project is tested using GoogleTest.
  add_subdirectory("third_party/googletest")

  # GoogleTest triggers a missing field initializers warning.
  if(CPPUTILS_HAVE_NO_MISSING_FIELD_INITIALIZERS)
    set_property(TARGET gtest
        APPEND PROPERTY COMPILE_OPTIONS -Wno-missing-field-initializers)
    set_property(TARGET gmock
        APPEND PROPERTY COMPILE_OPTIONS -Wno-missing-field-initializers)
  endif(CPPUTILS_HAVE_NO_MISSING_FIELD_INITIALIZERS)

  if(CPPUTILS_HAVE_NO_IMPLICIT_INT_FLOAT_CONVERSION)
    set_property(TARGET gtest
        APPEND PROPERTY COMPILE_OPTIONS -Wno-implicit-int-float-conversion)
  endif(CPPUTILS_HAVE_NO_IMPLICIT_INT_FLOAT_CONVERSION)

  add_executable(unittest_common "")
  target_sources(unittest_common
    PRIVATE
      "test/test_common.cc"
  )
  target_link_libraries(unittest_common gmock_main gtest common)
  add_test(
    NAME unittest_common
    WORKING_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}"
    COMMAND "${EXECUTABLE_OUTPUT_PATH}/unittest_common")

endif(CPPUTILS_BUILD_TESTS)
