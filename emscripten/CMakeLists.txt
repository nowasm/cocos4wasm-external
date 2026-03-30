
# zlib: use Emscripten port via -sUSE_ZLIB=1 on cocos_engine (main CMakeLists.txt).
# Do not link a repo-local libz.a (not shipped under external/emscripten/libs).

list(APPEND CC_EXTERNAL_LIBS
    #freetype
    #jpeg
    #png
    #webp
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
