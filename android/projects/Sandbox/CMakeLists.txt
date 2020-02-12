cmake_minimum_required(VERSION 3.6.0 FATAL_ERROR)

set (NAME Sandbox)

set(SRC_DIR ../../../${NAME}/src)
set(HAZEL_DIR ../../../Hazel)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DHZ_DEBUG")

file(GLOB APP_SRC "${SRC_DIR}/*.cpp")

add_library(native_app_glue STATIC ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)

add_subdirectory(../Hazel ${CMAKE_SOURCE_DIR}/../Hazel)

set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

add_library(native-activity SHARED ${APP_SRC})

target_include_directories(native-activity PRIVATE ${ANDROID_NDK}/sources/android/native_app_glue)

include_directories(${HAZEL_DIR}/src)
include_directories(${HAZEL_DIR}/vendor)
include_directories(${HAZEL_DIR}/vendor/spdlog/include)
include_directories(${HAZEL_DIR}/vendor/glm)

target_link_libraries(
    native-activity
    android
    native_app_glue
    log
    Hazel
)