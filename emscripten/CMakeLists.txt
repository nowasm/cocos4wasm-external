
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
  list(APPEND CC_EXTERNAL_LIBS freetype)
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
