cmake_minimum_required(VERSION 3.16)

set(QUICKJS_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/../sources/quickjs")

add_library(quickjs STATIC
    ${QUICKJS_SRC_DIR}/dtoa.c
    ${QUICKJS_SRC_DIR}/libregexp.c
    ${QUICKJS_SRC_DIR}/libunicode.c
    ${QUICKJS_SRC_DIR}/quickjs.c
)

target_include_directories(quickjs PUBLIC
    ${QUICKJS_SRC_DIR}
)

target_compile_definitions(quickjs PRIVATE
    _GNU_SOURCE
    CONFIG_VERSION="0.8.0"
)

if(WIN32)
    target_compile_definitions(quickjs PRIVATE WIN32_LEAN_AND_MEAN _WIN32_WINNT=0x0601)
endif()

if(NOT EMSCRIPTEN AND NOT CMAKE_SYSTEM_NAME STREQUAL "WASI")
    find_package(Threads)
    if(CMAKE_THREAD_LIBS_INIT)
        target_link_libraries(quickjs PUBLIC ${CMAKE_THREAD_LIBS_INIT})
    endif()
endif()

find_library(M_LIBRARIES m)
if(M_LIBRARIES)
    target_link_libraries(quickjs PUBLIC m)
endif()

target_link_libraries(quickjs PUBLIC ${CMAKE_DL_LIBS})
