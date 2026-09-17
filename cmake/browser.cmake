get_target_property(browser_sources gauche SOURCES)
list(REMOVE_ITEM browser_sources src/net/rooms/http.cpp src/net_socket.cpp)
set_property(TARGET gauche PROPERTY SOURCES ${browser_sources})
target_sources(gauche PRIVATE src/browser/http.cpp src/browser/socket.cpp src/browser/inspection.cpp)
set_target_properties(gauche PROPERTIES OUTPUT_NAME teeming SUFFIX .js)
target_link_options(gauche PRIVATE
    -sASYNCIFY=1 -sASSERTIONS=1 -sASYNCIFY_STACK_SIZE=131072
    -sALLOW_MEMORY_GROWTH=1 -sSTACK_SIZE=8388608 -sINITIAL_MEMORY=134217728
    -sMODULARIZE=1 -sEXPORT_ES6=1 -sEXPORT_NAME=createTeeming
    -sENVIRONMENT=web -sFORCE_FILESYSTEM=1 -sEXIT_RUNTIME=0
    -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2
    "-sEXPORTED_RUNTIME_METHODS=['FS','callMain','requestFullscreen']"
    -lidbfs.js)
# Bundled HarfBuzz's older warning policy conflicts with current Clang templates.
if(TARGET harfbuzz)
    target_compile_definitions(harfbuzz PRIVATE HB_NO_PRAGMA_GCC_DIAGNOSTIC_ERROR)
endif()
