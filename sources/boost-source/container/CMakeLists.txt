# Copyright 2020, 2021 Peter Dimov
# Distributed under the Boost Software License, Version 1.0.
# https://www.boost.org/LICENSE_1_0.txt

cmake_minimum_required(VERSION 3.5...3.16)

project(boost_container VERSION "${BOOST_SUPERPROJECT_VERSION}" LANGUAGES C CXX)

if(EMSCRIPTEN)
  # wasm is single-threaded; synchronized_pool_resource pulls in thread_mutex.hpp
  # which has Windows-only __declspec/__stdcall declarations.  Exclude it and
  # disable thread detection so boost config doesn't try to pick a threading API.
  add_library(boost_container
    alloc_lib.c
    dlmalloc.cpp
    global_resource.cpp
    monotonic_buffer_resource.cpp
    pool_resource.cpp
    unsynchronized_pool_resource.cpp
  )
  target_compile_definitions(boost_container PUBLIC BOOST_DISABLE_THREADS)
else()
  add_library(boost_container
    alloc_lib.c
    dlmalloc.cpp
    global_resource.cpp
    monotonic_buffer_resource.cpp
    pool_resource.cpp
    synchronized_pool_resource.cpp
    unsynchronized_pool_resource.cpp
  )
endif()

add_library(Boost::container ALIAS boost_container)

target_compile_definitions(boost_container
  PUBLIC BOOST_CONTAINER_NO_LIB
  # Source files already define BOOST_CONTAINER_SOURCE
  # PRIVATE BOOST_CONTAINER_SOURCE
)

if(BUILD_SHARED_LIBS)
  target_compile_definitions(boost_container PUBLIC BOOST_CONTAINER_DYN_LINK)
else()
  target_compile_definitions(boost_container PUBLIC BOOST_CONTAINER_STATIC_LINK)
endif()

if(BUILD_TESTING AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/test/CMakeLists.txt")
  add_subdirectory(test)
endif()

if(APPLE)
  set_target_properties(boost_container PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/archives
  )
endif()
