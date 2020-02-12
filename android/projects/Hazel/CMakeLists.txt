cmake_minimum_required(VERSION 3.6.0 FATAL_ERROR)

set(HAZEL_DIR ../../../Hazel)
set(SRC_DIR ${HAZEL_DIR}/src)

file(GLOB_RECURSE HAZEL_SRC "${SRC_DIR}/*.cpp")
list(FILTER HAZEL_SRC EXCLUDE REGEX "${SRC_DIR}/Platform/Windows/*")

add_subdirectory(vendor/imgui ${CMAKE_SOURCE_DIR}/vendor/imgui)

add_library(stb_image STATIC ${HAZEL_DIR}/vendor/stb_image/stb_image.cpp)

add_library(Hazel SHARED ${HAZEL_SRC})

include_directories(${HAZEL_DIR}/src)
include_directories(${HAZEL_DIR}/vendor)
include_directories(${HAZEL_DIR}/vendor/imgui)
include_directories(${HAZEL_DIR}/vendor/spdlog/include)
include_directories(${HAZEL_DIR}/vendor/glm)
include_directories(${HAZEL_DIR}/vendor/stb_image)
include_directories(${ANDROID_NDK}/sources/android/native_app_glue)

target_link_Libraries(
	Hazel
	Imgui
	stb_image
	android
	EGL
	GLESv3
	log
)