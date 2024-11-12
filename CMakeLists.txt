# 指定 cmake 最小版本
cmake_minimum_required(VERSION 3.5)

# 指定项目名称
project(storage
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

option(ENABLE_SHARED "build shared libraries" ON)
include(CommonChecks)

# fmt
find_package(fmt 6.0.0 QUIET)
if(fmt_FOUND)
  include_directories(SYSTEM "${fmt_INCLUDE_DIR}")
else(fmt_FOUND)
  message(FATAL_ERROR "fmt library not found")
endif(fmt_FOUND)
list(APPEND target_deps fmt::fmt)

# Boost
option(WITH_SYSTEM_BOOST "require and build with system Boost" ON)

## Boost::thread depends on Boost::atomic, so list it explicitly.
set(BOOST_COMPONENTS
  atomic chrono thread system regex random program_options date_time
  iostreams context coroutine timer filesystem)
set(BOOST_HEADER_COMPONENTS container)

set(Boost_USE_MULTITHREADED ON)
# require minimally the bundled version
if(WITH_SYSTEM_BOOST)
  if(ENABLE_SHARED)
    set(Boost_USE_STATIC_LIBS OFF)
  else()
    set(Boost_USE_STATIC_LIBS ON)
  endif()
  if(BOOST_ROOT AND CMAKE_LIBRARY_ARCHITECTURE)
    set(BOOST_LIBRARYDIR "${BOOST_ROOT}/lib/${CMAKE_LIBRARY_ARCHITECTURE}")
  endif()
  find_package(Boost 1.73 COMPONENTS ${BOOST_COMPONENTS} REQUIRED)
  if(NOT ENABLE_SHARED)
    set_property(TARGET Boost::iostreams APPEND PROPERTY
      INTERFACE_LINK_LIBRARIES ZLIB::ZLIB)
  endif()
else()
  set(BOOST_J 1 CACHE STRING
    "max jobs for Boost build") # override w/-DBOOST_J=<n>
  set(Boost_USE_STATIC_LIBS ON)
  include(BuildBoost)
  build_boost(1.75
    COMPONENTS ${BOOST_COMPONENTS} ${BOOST_HEADER_COMPONENTS})
endif()
include_directories(BEFORE SYSTEM ${Boost_INCLUDE_DIRS})
add_definitions(-DBOOST_ASIO_DISABLE_THREAD_KEYWORD_EXTENSION)
if(Boost_VERSION VERSION_GREATER_EQUAL 1.74)
  add_definitions(-DBOOST_ASIO_USE_TS_EXECUTOR_AS_DEFAULT)
endif()
list(APPEND target_deps
  Boost::thread
  Boost::system
  Boost::random
  Boost::program_options
  Boost::date_time
  Boost::iostreams
  Boost::filesystem)
add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-fpermissive>)

# arch
include(SIMDExt)
if(HAVE_INTEL)
  set(object_format "elf64")
  set(CMAKE_ASM_FLAGS "-f ${object_format}")
  set(CMAKE_ASM_COMPILER ${CMAKE_SOURCE_DIR}/tool/nasm-wrapper)
  # The native tools might be located even when cross compiling, which
  # might not work in this case (especially when targeting Windows).
  include(CheckNasm)
  check_nasm_support(${object_format}
    HAVE_NASM_X64
    HAVE_NASM_X64_AVX2
    HAVE_NASM_X64_AVX512)
  if(HAVE_NASM_X64)
    add_definitions(-DHAVE_NASM_X64)
  endif(HAVE_NASM_X64)
elseif(HAVE_ARM)
  if(HAVE_ARMV8_CRC_CRYPTO_INTRINSICS)
    add_definitions(-DHAVE_ARMV8_CRC_CRYPTO_INTRINSICS)
    add_definitions(-DHAVE_ARMV8_CRC)
    add_definitions(-DHAVE_ARMV8_CRYPTO)
  endif(HAVE_ARMV8_CRC_CRYPTO_INTRINSICS)
elseif(HAVE_PPC64LE)
  add_definitions(-DHAVE_PPC64LE)
endif(HAVE_INTEL)

set(arch_srcs
  common/arch/probe.cc)
if(HAVE_ARM)
  list(APPEND arch_srcs common/arch/arm.cc)
elseif(HAVE_INTEL)
  list(APPEND arch_srcs common/arch/intel.cc)
elseif(HAVE_PPC64LE OR HAVE_PPC64 OR HAVE_PPC)
  list(APPEND arch_srcs common/arch/ppc.cc)
endif()

set(crc32_srcs
  common/crc/crc32.cc
  common/crc/crc32_sctp.cc
  common/crc/crc32_intel_baseline.cc)
if(HAVE_INTEL)
  list(APPEND crc32_srcs
    common/crc/crc32_intel_fast.cc)
  if(HAVE_NASM_X64)
    set(CMAKE_ASM_FLAGS "-i ${PROJECT_SOURCE_DIR}/third_party/isa-l/include/ ${CMAKE_ASM_FLAGS}")
    list(APPEND crc32_srcs
      ${PROJECT_SOURCE_DIR}/third_party/isa-l/crc/crc32_iscsi_00.asm
      common/crc/crc32_intel_fast_zero_asm.s)
  endif(HAVE_NASM_X64)
elseif(HAVE_ARMV8_CRC)
  list(APPEND crc32_srcs
    common/crc/crc32_aarch64.cc)
elseif(HAVE_POWER8)
  list(APPEND crc32_srcs
    common/crc/crc32_ppc.cc)
  if(HAVE_PPC64LE)
    list(APPEND crc32_srcs
      common/crc/crc32_ppc_asm.S
      common/crc/crc32_ppc_fast_zero_asm.S)
  endif(HAVE_PPC64LE)
endif(HAVE_INTEL)

set(common_srcs
  common/dout.cc
  common/backtrace.cc
  common/assertion.cc
  common/formatter.cc
  common/reverse.cc
  common/page.cc
  common/mempool.cc
  common/error_code.cc
  common/environment.cc
  common/armor.cc
  common/safe_io.cc
  common/buffer.cc
  common/uuid.cc)

# 设置库的源文件
set(SOURCES
  ${arch_srcs}
  ${crc32_srcs}
  ${common_srcs}
)

# 创建动态库
add_library(common SHARED ${SOURCES})
target_link_libraries(common ${target_deps})


# 测试
# Used by googletest.
check_cxx_compiler_flag(-Wno-missing-field-initializers
                        HAVE_NO_MISSING_FIELD_INITIALIZERS)
check_cxx_compiler_flag(-Wno-implicit-int-float-conversion
                        HAVE_NO_IMPLICIT_INT_FLOAT_CONVERSION)

option(BUILD_TESTS "Build storage's own tests." ON)
if(BUILD_TESTS)

  enable_testing()

  # This project is tested using GoogleTest.
  add_subdirectory("third_party/googletest")

  # GoogleTest triggers a missing field initializers warning.
  if(HAVE_NO_MISSING_FIELD_INITIALIZERS)
    set_property(TARGET gtest
        APPEND PROPERTY COMPILE_OPTIONS -Wno-missing-field-initializers)
    set_property(TARGET gmock
        APPEND PROPERTY COMPILE_OPTIONS -Wno-missing-field-initializers)
  endif(HAVE_NO_MISSING_FIELD_INITIALIZERS)

  if(HAVE_NO_IMPLICIT_INT_FLOAT_CONVERSION)
    set_property(TARGET gtest
        APPEND PROPERTY COMPILE_OPTIONS -Wno-implicit-int-float-conversion)
  endif(HAVE_NO_IMPLICIT_INT_FLOAT_CONVERSION)

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

endif(BUILD_TESTS)
