cmake_minimum_required(VERSION 3.21)
project(maplibre-jni)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find JNI
find_package(JNI REQUIRED)

# Enable position-independent code
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Configure MapLibre Native build options
set(MLN_WITH_OPENGL ON CACHE BOOL "" FORCE)
set(MLN_WITH_RTTI ON CACHE BOOL "" FORCE)
set(MLN_WITH_WERROR OFF CACHE BOOL "" FORCE)
set(MLN_WITH_GLFW OFF CACHE BOOL "" FORCE)

# Add MapLibre Native
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../vendor/maplibre-native maplibre-native)

# Create our JNI library
add_library(maplibre-jni SHARED
    src/main/cpp/jni_helpers.hpp
    src/main/cpp/jni_size.cpp
    src/main/cpp/jni_latlng.cpp
    src/main/cpp/jni_edgeinsets.cpp
    src/main/cpp/jni_screencoordinate.cpp
    src/main/cpp/jni_cameraoptions.cpp
    src/main/cpp/jni_mapoptions.cpp
    src/main/cpp/jni_resourceoptions.cpp
    src/main/cpp/jni_clientoptions.cpp
    src/main/cpp/jni_run_loop.cpp
    src/main/cpp/jni_map_observer.cpp
    src/main/cpp/jni_native_map_observer.cpp
    src/main/cpp/jni_maplibre_map.cpp
    src/main/cpp/jni_jogl_backend.cpp
    src/main/cpp/jni_jogl_renderer_backend.cpp
    src/main/cpp/jni_jogl_context.cpp
    src/main/cpp/jni_jogl_renderer_frontend.cpp
    src/main/cpp/jni_jogl_renderer_frontend_impl.cpp
)

# Include directories
target_include_directories(maplibre-jni PRIVATE
    ${JNI_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/maplibre-native/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/maplibre-native/platform/default/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/maplibre-native/src
)

# Compile options (will inherit from mbgl-compiler-options)
target_compile_options(maplibre-jni PRIVATE
    -frtti
    -fexceptions
)

# Link against MapLibre (this brings in all dependencies)
target_link_libraries(maplibre-jni
    PRIVATE
        Mapbox::Map
        mbgl-compiler-options
        ${JNI_LIBRARIES}
)

# Platform-specific OpenGL linking
if(NOT WIN32)
    find_package(OpenGL REQUIRED)
    target_link_libraries(maplibre-jni PRIVATE OpenGL::GL)
endif()

# Set output directory
set_target_properties(maplibre-jni PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build/lib/main/shared
)
