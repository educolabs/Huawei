cmake_minimum_required(VERSION 3.10)
project(hwfwtool VERSION 1.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

find_package(ZLIB REQUIRED)

set(HWFWTOOL_SOURCES
    src/main.c
    src/firmware.c
    src/blocks.c
    src/compress.c
    src/crypto.c
    src/config.c
    src/cli.c
    src/utils.c
)

set(HWFWTOOL_HEADERS
    include/hwfwtool/common.h
    include/hwfwtool/firmware.h
    include/hwfwtool/blocks.h
    include/hwfwtool/compress.h
    include/hwfwtool/crypto.h
    include/hwfwtool/config.h
    include/hwfwtool/cli.h
    include/hwfwtool/utils.h
)

add_executable(hwfwtool ${HWFWTOOL_SOURCES} ${HWFWTOOL_HEADERS})

target_include_directories(hwfwtool PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${ZLIB_INCLUDE_DIRS}
)

target_link_libraries(hwfwtool PRIVATE
    ZLIB::ZLIB
)

if(WIN32)
    target_link_libraries(hwfwtool PRIVATE ws2_32)
endif()

if(UNIX AND NOT APPLE)
    target_link_libraries(hwfwtool PRIVATE m)
endif()

install(TARGETS hwfwtool DESTINATION bin)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(hwfwtool PRIVATE HWFWTOOL_DEBUG=1)
endif()

message(STATUS "hwfwtool v${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C compiler: ${CMAKE_C_COMPILER}")
message(STATUS "ZLIB found: ${ZLIB_FOUND}")
