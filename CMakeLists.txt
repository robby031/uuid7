cmake_minimum_required(VERSION 3.16)
project(uuid7
    VERSION 1.0.5
    DESCRIPTION "UUID v7 library with monotonic ordering"
    LANGUAGES C
)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(UUID7_SOURCES
    src/uuid7.c
    src/clock.c
    src/prng.c
)

add_library(uuid7 STATIC ${UUID7_SOURCES})
target_include_directories(uuid7 PUBLIC  include)
target_include_directories(uuid7 PRIVATE src)

add_library(uuid7_shared SHARED ${UUID7_SOURCES})
target_include_directories(uuid7_shared PUBLIC  include)
target_include_directories(uuid7_shared PRIVATE src)
set_target_properties(uuid7_shared PROPERTIES
    OUTPUT_NAME uuid7
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
)

include(GNUInstallDirs)

install(TARGETS uuid7 uuid7_shared
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

set(prefix ${CMAKE_INSTALL_PREFIX})
set(exec_prefix ${prefix})
set(libdir ${CMAKE_INSTALL_FULL_LIBDIR})
set(includedir ${CMAKE_INSTALL_FULL_INCLUDEDIR})

configure_file(
    ${PROJECT_SOURCE_DIR}/uuid7.pc.in
    ${PROJECT_BINARY_DIR}/uuid7.pc
    @ONLY
)

install(FILES ${PROJECT_BINARY_DIR}/uuid7.pc
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

option(BUILD_TESTS "Build unit tests" OFF)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

option(BUILD_BENCHMARKS "Build benchmarks" OFF)
if(BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()

add_executable(basic_usage examples/basic_usage.c)
target_link_libraries(basic_usage PRIVATE uuid7)