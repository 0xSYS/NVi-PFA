cmake_minimum_required(VERSION 3.6)

project(my_app)


option(EXE_TYPE "Build core aplication as executable" OFF)

if(NOT TARGET SDL3::SDL3)
    find_package(SDL3 CONFIG)
endif()

if(NOT TARGET SDL3::SDL3)
    find_library(SDL3_LIBRARY NAMES "SDL3")
    find_path(SDL3_INCLUDE_DIR NAMES "SDL3/SDL.h")
    add_library(SDL3::SDL3 UNKNOWN IMPORTED)
    set_property(TARGET SDL3::SDL3 PROPERTY IMPORTED_LOCATION "${SDL3_LIBRARY}")
    set_property(TARGET SDL3::SDL3 PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${SDL3_INCLUDE_DIR}")
endif()

if(NOT TARGET SDL3::SDL3)
    message(FATAL_ERROR "Cannot find SDL3.

Possible ways to fix this:
- Use a SDL3 Android aar archive, and configure gradle to use it: prefab is required.
- Add add_subdirectory(path/to/SDL) to your CMake script, and make sure a vendored SDL is present there.
")
endif()

set(CMAKE_CXX_STANDARD 17)

#add_compile_options(-UNON_ANDROID)

#link_directories(${CMAKE_SOURCE_DIR}extern/lib)
#link_directories(${CMAKE_SOURCE_DIR}/../libs/arm64-v8a)


if(EXE_TYPE)
message(STATUS "Building as executable")
else()

# Import BASS libs
add_library(bass SHARED IMPORTED)
set_target_properties(bass PROPERTIES IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/../libs/arm64-v8a/libbass.so")

add_library(bassmidi SHARED IMPORTED)
set_target_properties(bassmidi PROPERTIES IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/../libs/arm64-v8a/libbassmidi.so")
endif()


include_directories(".")


if(EXE_TYPE)

add_definitions(-DNON_ANDROID=1)

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall")
set(CMAKE_C_FLAGS_RELEASE "-O3 -Wall")
add_executable(nvi-pfa
    extern/imgui_sdl3/imgui_impl_sdl3.cpp
    extern/imgui_sdl3/imgui_impl_sdlrenderer3.cpp
    extern/imgui/imgui.cpp
    extern/imgui/imgui_demo.cpp
    extern/imgui/imgui_tables.cpp
    extern/imgui/imgui_widgets.cpp
    extern/imgui/imgui_draw.cpp
    audio_effects.cxx
    Config_Utils.cxx
    file_utils.cxx
    MIDI.cxx
    Nlist.cxx
    Player.cxx
    Sequ.cxx
    Utils.cxx
    canvas.cxx
    Gui.cxx
)
target_link_libraries(nvi-pfa PRIVATE SDL3::SDL3 bass bassmidi)
else()

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall")
set(CMAKE_C_FLAGS_RELEASE "-O3 -Wall")
add_library(main SHARED
    extern/imgui_sdl3/imgui_impl_sdl3.cpp
    extern/imgui_sdl3/imgui_impl_sdlrenderer3.cpp
    extern/imgui/imgui.cpp
    extern/imgui/imgui_demo.cpp
    extern/imgui/imgui_tables.cpp
    extern/imgui/imgui_widgets.cpp
    extern/imgui/imgui_draw.cpp
    audio_effects.cxx
    Config_Utils.cxx
    file_utils.cxx
    MIDI.cxx
    Nlist.cxx
    Player.cxx
    Sequ.cxx
    Utils.cxx
    canvas.cxx
    Gui.cxx
)
target_link_libraries(main PRIVATE SDL3::SDL3 bass bassmidi log)

endif()
