LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

# Add your application source files here...
LOCAL_SRC_FILES := \
extern/imgui_sdl3/imgui_impl_sdl3.cpp \
extern/imgui_sdl3/imgui_impl_sdlrenderer3.cpp \
extern/imgui/imgui.cpp \
extern/imgui/imgui_demo.cpp \
extern/imgui/imgui_tables.cpp \
extern/imgui/imgui_widgets.cpp \
extern/imgui/imgui_draw.cpp \
Config_Utils.cxx \
file_utils.cxx \
MIDI.cxx \
Nlist.cxx \
Player.cxx \
Sequ.cxx \
Utils.cxx \
canvas.cxx \
Gui.cxx \

LOCAL_SHARED_LIBRARIES := SDL3 SDL3-Headers
include $(BUILD_SHARED_LIBRARY)

# https://google.github.io/prefab/build-systems.html

# Add the prefab modules to the import path.
$(call import-add-path,/out)

# Import SDL3 so we can depend on it.
$(call import-module,prefab/SDL3)
