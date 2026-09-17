set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(GUB_SDL_DEPS fetch CACHE STRING "" FORCE)
set(GUB_WARN_AS_ERROR OFF CACHE BOOL "" FORCE)
set(SDLTTF_HARFBUZZ ON CACHE BOOL "" FORCE)
set(GUB_RUNTIME_PROJECT_SOURCE_DIR "/teeming" CACHE STRING "" FORCE)
# Catch malformed room replies in the browser, just as on native builds.
add_compile_options(-fexceptions)
add_link_options(-fexceptions)
