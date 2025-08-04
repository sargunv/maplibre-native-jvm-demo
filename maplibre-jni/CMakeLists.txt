cmake_minimum_required(VERSION 3.21)
project(maplibre-jni)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Path to generated JNI headers
set(JNI_HEADERS_DIR "" CACHE PATH "Path to generated JNI headers")

# Enable Objective-C++ on macOS
if(APPLE)
    enable_language(OBJCXX)
endif()

# Find JNI
find_package(JNI REQUIRED)

# Enable position-independent code
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Configure MapLibre Native build options based on platform
if(APPLE)
    # Use Metal backend on macOS
    set(MLN_WITH_METAL ON CACHE BOOL "" FORCE)
    set(MLN_WITH_OPENGL OFF CACHE BOOL "" FORCE)
    set(MLN_WITH_VULKAN OFF CACHE BOOL "" FORCE)
else()
    # Use Vulkan backend on Linux/Windows
    set(MLN_WITH_METAL OFF CACHE BOOL "" FORCE)
    set(MLN_WITH_OPENGL OFF CACHE BOOL "" FORCE)
    set(MLN_WITH_VULKAN ON CACHE BOOL "" FORCE)
endif()

set(MLN_WITH_RTTI ON CACHE BOOL "" FORCE)
set(MLN_WITH_WERROR OFF CACHE BOOL "" FORCE)
set(MLN_WITH_GLFW OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)

# Add MapLibre Native
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../vendor/maplibre-native maplibre-native)

# Create our JNI library
set(MAPLIBRE_JNI_SOURCES
    src/main/cpp/jni_helpers.hpp
    src/main/cpp/conversions/jni_latlng_conversions.cpp
    src/main/cpp/conversions/jni_size_conversions.cpp
    src/main/cpp/conversions/jni_screencoordinate_conversions.cpp
    src/main/cpp/conversions/jni_edgeinsets_conversions.cpp
    src/main/cpp/conversions/jni_cameraoptions_conversions.cpp
    src/main/cpp/conversions/jni_mapoptions_conversions.cpp
    src/main/cpp/conversions/jni_clientoptions_conversions.cpp
    src/main/cpp/conversions/jni_tileserveroptions_conversions.cpp
    src/main/cpp/conversions/jni_resourceoptions_conversions.cpp
    src/main/cpp/jni_map_observer.cpp
    src/main/cpp/jni_native_map_observer.cpp
    src/main/cpp/jni_maplibre_map.cpp
    # New unified renderer architecture
    src/main/cpp/jni_awt_canvas_renderer.cpp
    src/main/cpp/jni_awt_backend_factory.cpp
)

# Add platform-specific backend sources
if(APPLE)
    # Metal backend for macOS
    list(APPEND MAPLIBRE_JNI_SOURCES 
        src/main/cpp/jni_awt_metal_backend.mm
    )
else()
    # OpenGL ES backend for Linux/Windows (stub for now)
    list(APPEND MAPLIBRE_JNI_SOURCES 
        src/main/cpp/jni_awt_gl_backend.cpp
    )
endif()

add_library(maplibre-jni SHARED ${MAPLIBRE_JNI_SOURCES})

# Include directories
target_include_directories(maplibre-jni PRIVATE
    ${JNI_INCLUDE_DIRS}
    ${JNI_HEADERS_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/maplibre-native/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/maplibre-native/platform/default/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/maplibre-native/src
)

# Add Metal-cpp headers for macOS
if(APPLE)
    target_include_directories(maplibre-jni PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/maplibre-native/vendor/metal-cpp
    )
endif()


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

# Link against JAWT for native window access
if(APPLE)
    find_library(JAWT_LIBRARY jawt HINTS ${JAVA_HOME}/lib ${JAVA_HOME}/jre/lib)
    if(JAWT_LIBRARY)
        target_link_libraries(maplibre-jni PRIVATE ${JAWT_LIBRARY})
    endif()
    # Link against necessary macOS frameworks
    target_link_libraries(maplibre-jni PRIVATE 
        "-framework Cocoa"
        "-framework QuartzCore"
        "-framework Metal"
    )
else()
    # Linux
    find_library(JAWT_LIBRARY jawt HINTS ${JAVA_HOME}/lib ${JAVA_HOME}/jre/lib/amd64)
    if(JAWT_LIBRARY)
        target_link_libraries(maplibre-jni PRIVATE ${JAWT_LIBRARY})
    endif()
endif()

# Platform-specific graphics library linking
if(NOT APPLE)
    # Linux/Windows: Vulkan support (MapLibre Native will handle Vulkan linking)
    # No additional linking needed here as Vulkan is handled by MapLibre Native
endif()

# Set output directory
set_target_properties(maplibre-jni PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build/lib/main/shared
)
