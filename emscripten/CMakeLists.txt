
# zlib: use Emscripten port via -sUSE_ZLIB=1 on cocos_engine (main CMakeLists.txt).
# Do not link a repo-local libz.a (not shipped under external/emscripten/libs).

add_library(jpeg INTERFACE)
target_link_options(jpeg INTERFACE -sUSE_LIBJPEG=1)

add_library(png INTERFACE)
target_link_options(png INTERFACE -sUSE_LIBPNG=1)

if(USE_WEBP)
  set(_webp_lib "${CMAKE_CURRENT_LIST_DIR}/libs/libwebp/libwebp.a")
  if(EXISTS "${_webp_lib}")
    add_library(webp STATIC IMPORTED GLOBAL)
    set_target_properties(webp PROPERTIES
      IMPORTED_LOCATION "${_webp_lib}"
      INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include"
    )
    list(APPEND CC_EXTERNAL_LIBS webp)
  else()
    message(STATUS "WebP: libwebp.a not found at ${_webp_lib}, skipping (CC_USE_WEBP will be 0)")
    set(USE_WEBP OFF)
  endif()
endif()

if(USE_AUDIO)
  add_library(openal INTERFACE)
  target_link_options(openal INTERFACE -lopenal)

  add_library(ogg INTERFACE)
  target_link_options(ogg INTERFACE -sUSE_OGG=1)
  target_compile_options(ogg INTERFACE -sUSE_OGG=1)

  add_library(vorbis INTERFACE)
  target_link_options(vorbis INTERFACE -sUSE_VORBIS=1)
  target_compile_options(vorbis INTERFACE -sUSE_VORBIS=1)

  add_library(mpg123 INTERFACE)
  target_link_options(mpg123 INTERFACE -sUSE_MPG123=1)
  target_compile_options(mpg123 INTERFACE -sUSE_MPG123=1)

  list(APPEND CC_EXTERNAL_LIBS openal ogg vorbis mpg123)
endif()

if(USE_DEBUG_RENDERER)
  add_library(freetype INTERFACE)
  target_link_options(freetype INTERFACE -sUSE_FREETYPE=1)
  target_compile_options(freetype INTERFACE -sUSE_FREETYPE=1)

  # Headers: .../sysroot/include/freetype2/freetype/ft2build.h
  # Prefer sysroot next to this emcc (emsdk/upstream/emscripten/cache/sysroot): that is
  # where embuilder often installs, even when EMSCRIPTEN_SYSROOT / em-config points at
  # another cache (e.g. under %USERPROFILE%\.emscripten_cache).
  get_filename_component(_em_cc_dir "${CMAKE_C_COMPILER}" DIRECTORY)
  file(TO_CMAKE_PATH "${_em_cc_dir}" _em_cc_dir)
  set(_ft_bundled_sysroot "${_em_cc_dir}/cache/sysroot")

  set(_ft_sysroot "")
  if(EXISTS "${_ft_bundled_sysroot}/include/freetype2/freetype/ft2build.h"
      OR EXISTS "${_ft_bundled_sysroot}/include/freetype2/ft2build.h")
    set(_ft_sysroot "${_ft_bundled_sysroot}")
  elseif(DEFINED EMSCRIPTEN_SYSROOT AND NOT "${EMSCRIPTEN_SYSROOT}" STREQUAL "")
    set(_ft_sysroot "${EMSCRIPTEN_SYSROOT}")
  endif()
  if("${_ft_sysroot}" STREQUAL "")
    set(_emcfg "")
    if(WIN32 AND EXISTS "${_em_cc_dir}/em-config.bat")
      set(_emcfg "${_em_cc_dir}/em-config.bat")
    elseif(EXISTS "${_em_cc_dir}/em-config")
      set(_emcfg "${_em_cc_dir}/em-config")
    endif()
    if(_emcfg)
      execute_process(
        COMMAND "${_emcfg}" CACHE
        OUTPUT_VARIABLE _emcache_root
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE _emcfg_rc
      )
      if(_emcfg_rc EQUAL 0 AND NOT "${_emcache_root}" STREQUAL "")
        file(TO_CMAKE_PATH "${_emcache_root}" _emcache_root)
        set(_ft_sysroot "${_emcache_root}/sysroot")
      endif()
    endif()
  endif()
  if("${_ft_sysroot}" STREQUAL "" AND DEFINED ENV{EM_CACHE} AND NOT "$ENV{EM_CACHE}" STREQUAL "")
    file(TO_CMAKE_PATH "$ENV{EM_CACHE}" _emcache_env)
    set(_ft_sysroot "${_emcache_env}/sysroot")
  endif()
  if("${_ft_sysroot}" STREQUAL "" AND DEFINED EMSCRIPTEN_ROOT_PATH AND NOT "${EMSCRIPTEN_ROOT_PATH}" STREQUAL "")
    file(TO_CMAKE_PATH "${EMSCRIPTEN_ROOT_PATH}" _emroot_norm)
    set(_ft_sysroot "${_emroot_norm}/cache/sysroot")
    unset(_emroot_norm)
  endif()
  if("${_ft_sysroot}" STREQUAL "")
    set(_ft_sysroot "${_ft_bundled_sysroot}")
  endif()

  file(TO_CMAKE_PATH "${_ft_sysroot}" _ft_sysroot)
  set(_FREETYPE2_INC "${_ft_sysroot}/include/freetype2")
  file(TO_CMAKE_PATH "${_FREETYPE2_INC}" _FREETYPE2_INC)
  unset(_ft_bundled_sysroot)

  set(_FT2BUILD_H "${_FREETYPE2_INC}/freetype/ft2build.h")
  if(NOT EXISTS "${_FT2BUILD_H}")
    set(_FT2BUILD_H "${_FREETYPE2_INC}/ft2build.h")
  endif()

  # Headers are installed when the freetype *port* is built (embuilder / first link).
  # A bare `em++ -c -sUSE_FREETYPE=1` probe does not reliably create them before clang
  # runs, so run embuilder explicitly (same layout as emcc on PATH).
  if(NOT EXISTS "${_FT2BUILD_H}")
    get_filename_component(_em_bin_for_builder "${CMAKE_C_COMPILER}" DIRECTORY)
    set(_emb_cmd "")
    # emsdk may ship only embuilder.py (run with bundled or PATH python), not embuilder.bat.
    if(WIN32 AND EXISTS "${_em_bin_for_builder}/embuilder.bat")
      set(_emb_cmd "${_em_bin_for_builder}/embuilder.bat" build freetype)
    elseif(WIN32 AND EXISTS "${_em_bin_for_builder}/embuilder.cmd")
      set(_emb_cmd "${_em_bin_for_builder}/embuilder.cmd" build freetype)
    elseif(EXISTS "${_em_bin_for_builder}/embuilder" AND NOT WIN32)
      set(_emb_cmd "${_em_bin_for_builder}/embuilder" build freetype)
    elseif(EXISTS "${_em_bin_for_builder}/embuilder.py")
      set(_emb_py "")
      if(DEFINED ENV{EMSDK})
        file(GLOB _emb_py_glob LIST_DIRECTORIES false "$ENV{EMSDK}/python/*/python.exe")
        if(_emb_py_glob)
          list(GET _emb_py_glob 0 _emb_py)
        endif()
      endif()
      if(_emb_py STREQUAL "")
        find_program(_emb_py NAMES python3 python python3.exe python.exe)
      endif()
      if(_emb_py AND NOT _emb_py MATCHES "NOTFOUND")
        set(_emb_cmd "${_emb_py}" "${_em_bin_for_builder}/embuilder.py" build freetype)
      endif()
    endif()
    if(_emb_cmd STREQUAL "")
      find_program(_emb_shim NAMES embuilder embuilder.bat embuilder.cmd)
      if(_emb_shim AND NOT _emb_shim MATCHES "NOTFOUND")
        set(_emb_cmd "${_emb_shim}" build freetype)
      endif()
    endif()
    if(NOT _emb_cmd STREQUAL "")
      message(STATUS "Emscripten: building freetype port (embuilder)...")
      execute_process(
        COMMAND ${_emb_cmd}
        RESULT_VARIABLE _emb_rc
        OUTPUT_VARIABLE _emb_out
        ERROR_VARIABLE _emb_err
      )
      string(CONCAT _emb_log "${_emb_out}" "${_emb_err}")
      if(NOT _emb_rc EQUAL 0)
        message(FATAL_ERROR
          "embuilder build freetype failed (exit ${_emb_rc}). Install the port from an emsdk shell if needed.\n"
          "${_emb_log}")
      endif()
    else()
      message(FATAL_ERROR
        "FreeType headers missing and could not run embuilder (no embuilder.bat/cmd, embuilder.py + python, or embuilder on PATH).\n"
        "Expected near: ${_em_bin_for_builder}\n"
        "Run manually in an activated emsdk shell: python embuilder.py build freetype")
    endif()
    unset(_emb_py)
    unset(_emb_py_glob)
    unset(_emb_shim)
    unset(_emb_cmd)
  endif()

  # embuilder may write to the cache from ~/.emscripten / EM_CACHE, while EMSCRIPTEN_SYSROOT
  # from the toolchain points at upstream/emscripten/cache — resolve the real header path.
  if(NOT EXISTS "${_FT2BUILD_H}")
    get_filename_component(_em_em_dir "${CMAKE_C_COMPILER}" DIRECTORY)
    set(_ft_hit "")
    foreach(_p IN ITEMS
        "${_FREETYPE2_INC}/freetype/ft2build.h"
        "${_FREETYPE2_INC}/ft2build.h"
        "${_em_em_dir}/cache/sysroot/include/freetype2/freetype/ft2build.h"
        "${_em_em_dir}/cache/sysroot/include/freetype2/ft2build.h"
      )
      if(EXISTS "${_p}")
        set(_ft_hit "${_p}")
        break()
      endif()
    endforeach()
    if("${_ft_hit}" STREQUAL "" AND DEFINED EMSCRIPTEN_ROOT_PATH AND NOT "${EMSCRIPTEN_ROOT_PATH}" STREQUAL "")
      set(_p "${EMSCRIPTEN_ROOT_PATH}/cache/sysroot/include/freetype2/freetype/ft2build.h")
      if(EXISTS "${_p}")
        set(_ft_hit "${_p}")
      endif()
    endif()
    if("${_ft_hit}" STREQUAL "" AND DEFINED ENV{EM_CACHE} AND NOT "$ENV{EM_CACHE}" STREQUAL "")
      file(TO_CMAKE_PATH "$ENV{EM_CACHE}" _ecache)
      set(_p "${_ecache}/sysroot/include/freetype2/freetype/ft2build.h")
      if(EXISTS "${_p}")
        set(_ft_hit "${_p}")
      endif()
      unset(_ecache)
    endif()
    if("${_ft_hit}" STREQUAL "" AND DEFINED ENV{USERPROFILE} AND NOT "$ENV{USERPROFILE}" STREQUAL "")
      set(_p "$ENV{USERPROFILE}/.emscripten_cache/sysroot/include/freetype2/freetype/ft2build.h")
      if(EXISTS "${_p}")
        set(_ft_hit "${_p}")
      endif()
    endif()
    if("${_ft_hit}" STREQUAL "" AND DEFINED ENV{HOME} AND NOT "$ENV{HOME}" STREQUAL "")
      set(_p "$ENV{HOME}/.emscripten_cache/sysroot/include/freetype2/freetype/ft2build.h")
      if(EXISTS "${_p}")
        set(_ft_hit "${_p}")
      endif()
    endif()
    if("${_ft_hit}" STREQUAL "")
      # CMake 3.16: avoid ** glob; scan one level of subdirs (some caches use <root>/<hash>/sysroot/...).
      foreach(_root IN ITEMS "${_em_em_dir}/cache" "$ENV{USERPROFILE}/.emscripten_cache" "$ENV{HOME}/.emscripten_cache")
        if(NOT "${_root}" STREQUAL "" AND EXISTS "${_root}")
          set(_try "${_root}/sysroot/include/freetype2/freetype/ft2build.h")
          if(EXISTS "${_try}")
            set(_ft_hit "${_try}")
          endif()
          if("${_ft_hit}" STREQUAL "")
            file(GLOB _ft_subs LIST_DIRECTORIES true "${_root}/*")
            foreach(_sub IN LISTS _ft_subs)
              if(IS_DIRECTORY "${_sub}")
                set(_try "${_sub}/sysroot/include/freetype2/freetype/ft2build.h")
                if(EXISTS "${_try}")
                  set(_ft_hit "${_try}")
                  break()
                endif()
              endif()
            endforeach()
          endif()
        endif()
        if(NOT "${_ft_hit}" STREQUAL "")
          break()
        endif()
      endforeach()
      unset(_ft_subs)
      unset(_sub)
      unset(_try)
    endif()
    if(NOT "${_ft_hit}" STREQUAL "")
      get_filename_component(_d1 "${_ft_hit}" DIRECTORY)
      get_filename_component(_d1base "${_d1}" NAME)
      if(_d1base STREQUAL "freetype")
        get_filename_component(_FREETYPE2_INC "${_d1}" DIRECTORY)
      else()
        set(_FREETYPE2_INC "${_d1}")
      endif()
      get_filename_component(_incp "${_FREETYPE2_INC}" DIRECTORY)
      get_filename_component(_ft_sysroot "${_incp}" DIRECTORY)
      file(TO_CMAKE_PATH "${_FREETYPE2_INC}" _FREETYPE2_INC)
      set(_FT2BUILD_H "${_FREETYPE2_INC}/freetype/ft2build.h")
      if(NOT EXISTS "${_FT2BUILD_H}")
        set(_FT2BUILD_H "${_FREETYPE2_INC}/ft2build.h")
      endif()
      message(STATUS "Emscripten: using FreeType headers at ${_ft_hit}")
    endif()
    unset(_ft_hit)
    unset(_d1)
    unset(_d1base)
    unset(_incp)
    unset(_em_em_dir)
  endif()

  set(_FT2BUILD_H "${_FREETYPE2_INC}/freetype/ft2build.h")
  if(NOT EXISTS "${_FT2BUILD_H}")
    set(_FT2BUILD_H "${_FREETYPE2_INC}/ft2build.h")
  endif()
  if(NOT EXISTS "${_FT2BUILD_H}")
    message(FATAL_ERROR
      "FreeType headers not found (expected freetype/ft2build.h under sysroot include/freetype2). Tried: ${_FREETYPE2_INC}\n"
      "Build the port from an activated emsdk shell, then reconfigure: python embuilder.py build freetype")
  endif()

  # Use SYSTEM so Clang treats this like Emscripten's port (-isystem .../freetype2). Plain -I can lose
  # to --sysroot ordering and fail to resolve <freetype/ft2build.h> even when the tree exists.
  target_include_directories(freetype SYSTEM INTERFACE "${_FREETYPE2_INC}")
  if(EXISTS "${_FREETYPE2_INC}/freetype/ft2build.h")
    # standard layout
  elseif(EXISTS "${_FREETYPE2_INC}/ft2build.h")
    target_compile_definitions(freetype INTERFACE CC_FREETYPE_FT2BUILD_AT_ROOT=1)
  endif()

  unset(_ft_sysroot)
  unset(_FREETYPE2_INC)
  unset(_FT2BUILD_H)
  unset(_em_cc_dir)
  unset(_emcfg)
  unset(_emcache_root)
  unset(_emcfg_rc)
  unset(_emcache_env)
  unset(_em_bin_for_builder)
  unset(_emb_rc)
  unset(_emb_out)
  unset(_emb_err)
  unset(_emb_log)

  list(APPEND CC_EXTERNAL_LIBS freetype)
endif()

include(${CMAKE_CURRENT_LIST_DIR}/../quickjs/CMakeLists.txt)
list(APPEND CC_EXTERNAL_LIBS quickjs)

# Stub uv (libuv) target; jsb_global_init.cpp includes uv.h but
# actual libuv calls are only in debugger paths not reached on WASM.
if(NOT TARGET uv)
    add_library(uv INTERFACE)
    target_include_directories(uv INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
endif()

list(APPEND CC_EXTERNAL_LIBS
    jpeg
    png
    #curl
    #crypto
    #ssl
    #${tbb_libs_name}
    #${PhysXSDK}
)

set(ZLIB)

list(APPEND CC_EXTERNAL_INCLUDES
  ${CMAKE_CURRENT_LIST_DIR}/include
)
