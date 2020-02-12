cmake_minimum_required(VERSION 3.6.0 FATAL_ERROR)

set(HAZEL_DIR ../../../../../Hazel)
set(SRC_DIR ${HAZEL_DIR}/vendor/imgui)

add_library(Imgui STATIC
            ${SRC_DIR}/imconfig.h
            ${SRC_DIR}/imgui.h
            ${SRC_DIR}/imgui.cpp
            ${SRC_DIR}/imgui_draw.cpp
            ${SRC_DIR}/imgui_internal.h
            ${SRC_DIR}/imgui_widgets.cpp
            ${SRC_DIR}/imstb_rectpack.h
            ${SRC_DIR}/imstb_textedit.h
            ${SRC_DIR}/imstb_truetype.h
            ${SRC_DIR}/imgui_demo.cpp)

include_directories(${SRC_DIR})

target_link_libraries(Imgui)