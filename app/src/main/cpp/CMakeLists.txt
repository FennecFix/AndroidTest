cmake_minimum_required(VERSION 3.22.1)

project("testapplication")

add_library(native_app_glue STATIC ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)

set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

file(GLOB testapplication_SRC
        "*.h"
        "*.cpp")

add_subdirectory(thirdparty/glm)

add_library(testapplication SHARED ${testapplication_SRC})

target_include_directories(testapplication
        PRIVATE
        ${ANDROID_NDK}/sources/android/native_app_glue
        ${testapplication_SRC})

include_directories(${ANDROID_NDK}/sources/android/native_app_glue)

target_link_libraries(${CMAKE_PROJECT_NAME}
        android
        native_app_glue
        GLESv2
        EGL
        glm
        log)