cmake_minimum_required(VERSION 3.25)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VESRION 0.0.${PATCH_VERSION})
project(ehlib VERSION ${PROJECT_VESRION})

include(FetchContent)

FetchContent_Declare(
  mbedtls
  GIT_REPOSITORY https://github.com/Mbed-TLS/mbedtls.git
  GIT_TAG v3.4.1
)
FetchContent_MakeAvailable(mbedtls)

if(NOT mbedtls_POPULATED)
  FetchContent_Populate(mbedtls)
  add_subdirectory(${mbedtls_SOURCE_DIR} ${mbedtls_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)

FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
  FetchContent_Populate(googletest)
  add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

include_directories(include src)

add_library(
  mongoose
  STATIC
  src/mongoose/mongoose.c
)

add_library(
  ehlib
  STATIC
  src/ehlib.cpp
  src/params.cpp
  src/utils.cpp
  src/mimetype/mime_type.cpp
)

add_executable(
  test_ehlib_gtest 
  tests/test_ehlib_gtest.cpp
)

target_link_libraries(
  mongoose
  mbedtls
  mbedcrypto
  mbedx509)

target_link_libraries(
  ehlib
  mongoose)
  
if(WIN32)
  target_link_libraries(
    test_ehlib_gtest
    gtest 
    gtest_main
    ehlib
    ws2_32)
else()
  target_link_libraries(
    test_ehlib_gtest
    gtest 
    gtest_main
    ehlib)
endif()

set_target_properties(
  ehlib
  test_ehlib_gtest
  PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON
)

if (MSVC)
  target_compile_definitions(mongoose PRIVATE MG_TLS=1)

  target_compile_options(ehlib PRIVATE /W4)

  target_compile_options(test_ehlib_gtest PRIVATE /W4)
else ()
  target_compile_definitions(mongoose PRIVATE MG_TLS=1)

  target_link_options(ehlib PRIVATE -static-libgcc -static-libstdc++)

  target_link_options(test_ehlib_gtest PRIVATE -static-libgcc -static-libstdc++)

  target_compile_options(
    ehlib 
    PRIVATE
    -Wall 
    -Wextra 
    -pedantic 
  )

  target_compile_options(
    mongoose 
    PRIVATE
    -Wall 
    -Wextra 
    -pedantic 
  )

  target_compile_options(
    test_ehlib_gtest 
    PRIVATE
    -Wall 
    -Wextra 
    -pedantic 
  )
endif()

install(
  TARGETS ehlib mongoose mbedtls mbedcrypto mbedx509
  RUNTIME DESTINATION ehlib/bin COMPONENT applications
  LIBRARY DESTINATION ehlib/lib COMPONENT release
  ARCHIVE DESTINATION ehlib/lib COMPONENT release
)

install(
  DIRECTORY include
  DESTINATION ehlib
  COMPONENT release
  FILES_MATCHING
  PATTERN "*.h"
)

set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)
set(CPACK_COMPONENTS_ALL release)

# CPack area
if (WIN32)
    set(CPACK_GENERATOR "ZIP")
else ()
    set(CPACK_GENERATOR "TGZ")
endif()
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT https://github.com/4l3x777/ehlib)
include(CPack)

# Tests area
enable_testing()

add_test(
    NAME 
    unit_tests_test_ehlib_gtest 
    COMMAND 
    test_ehlib_gtest
)

