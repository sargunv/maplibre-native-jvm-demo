cmake_minimum_required(VERSION 3.21)
project(maplibre-jni)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(JNI_HEADERS_DIR "" CACHE PATH "Path to generated JNI headers")

if(APPLE)
    enable_language(OBJCXX)
endif()

find_package(JNI REQUIRED)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Add MapLibre Native
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../vendor/maplibre-native maplibre-native)

# Create our JNI library
set(MAPLIBRE_JNI_SOURCES
    src/main/cpp/jni_helpers.hpp
    src/main/cpp/conversions/latlng_conversions.cpp
    src/main/cpp/conversions/size_conversions.cpp
    src/main/cpp/conversions/screencoordinate_conversions.cpp
    src/main/cpp/conversions/edgeinsets_conversions.cpp
    src/main/cpp/conversions/cameraoptions_conversions.cpp
    src/main/cpp/conversions/mapoptions_conversions.cpp
    src/main/cpp/conversions/clientoptions_conversions.cpp
    src/main/cpp/conversions/tileserveroptions_conversions.cpp
    src/main/cpp/conversions/resourceoptions_conversions.cpp
    src/main/cpp/map_observer.cpp
    src/main/cpp/maplibre_map.cpp
    src/main/cpp/jni_renderer_frontend.cpp
    src/main/cpp/renderer_handle.cpp
    src/main/cpp/backend_factory.mm
    src/main/cpp/jawt_utils.mm
    src/main/cpp/surface_descriptors.cpp
)

# Old platform-specific backend sources are intentionally excluded from the build.
# New backends are created via backend_factory.* and accept native surface handles.

add_library(maplibre-jni SHARED ${MAPLIBRE_JNI_SOURCES})

# Include directories
# Mark external/vendor includes as SYSTEM to suppress warnings from their headers
target_include_directories(maplibre-jni
    PRIVATE
        ${JNI_INCLUDE_DIRS}
        ${JNI_HEADERS_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp
)

target_include_directories(maplibre-jni
    SYSTEM PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/maplibre-native/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/maplibre-native/platform/default/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/maplibre-native/src
)

# Add Windows-specific includes
if(WIN32)
    target_include_directories(maplibre-jni SYSTEM PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/maplibre-native/platform/windows/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/maplibre-native/platform/windows/vendor/vcpkg/installed/x64-windows/include
    )
endif()

# Add Metal-cpp headers for macOS
if(MLN_WITH_METAL)
    target_include_directories(maplibre-jni SYSTEM PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/maplibre-native/vendor/metal-cpp
    )
endif()


# Compile options (will inherit from mbgl-compiler-options)
target_compile_options(maplibre-jni PRIVATE
    -frtti
    -fexceptions
)

# Windows-specific definitions
if(WIN32)
    target_compile_definitions(maplibre-jni PRIVATE
        NOMINMAX  # Prevent min/max macro conflicts
    )
endif()

# Pass backend selection to C++ code
if(MLN_WITH_METAL)
    target_compile_definitions(maplibre-jni PRIVATE USE_METAL_BACKEND)
endif()

# Pass backend selection to C++ code
if(MLN_WITH_VULKAN)
    target_compile_definitions(maplibre-jni PRIVATE USE_VULKAN_BACKEND)
endif()

if(MLN_WITH_OPENGL)
    if (MLN_WITH_EGL)
        target_compile_definitions(maplibre-jni PRIVATE USE_EGL_BACKEND)
    elseif(WIN32)
        target_compile_definitions(maplibre-jni PRIVATE USE_WGL_BACKEND)
    elseif(UNIX AND NOT APPLE)
        target_compile_definitions(maplibre-jni PRIVATE USE_GLX_BACKEND)
    endif()
endif()

# Link against MapLibre (this brings in all dependencies)
target_link_libraries(maplibre-jni
    PRIVATE
        Mapbox::Map
        mbgl-compiler-options
        mbgl-vendor-unique_resource
        ${JNI_LIBRARIES}
)

# Link against JAWT for native window access
find_library(JAWT_LIBRARY jawt HINTS ${JAVA_HOME}/lib ${JAVA_HOME}/jre/lib)
if(JAWT_LIBRARY)
    target_link_libraries(maplibre-jni PRIVATE ${JAWT_LIBRARY})
endif()

if(APPLE)
    target_link_libraries(maplibre-jni PRIVATE 
        "-framework Cocoa"
        "-framework QuartzCore"
    )
endif()

if (MLN_WITH_METAL)
    target_link_libraries(maplibre-jni PRIVATE "-framework Metal")
endif()

if(UNIX AND NOT APPLE)
    find_package(X11 REQUIRED)
    target_link_libraries(maplibre-jni PRIVATE ${X11_LIBRARIES})
    target_include_directories(maplibre-jni SYSTEM PRIVATE ${X11_INCLUDE_DIR})
endif()

if(MLN_WITH_VULKAN)
    # Find Vulkan library
    find_package(Vulkan REQUIRED)
    target_link_libraries(maplibre-jni PRIVATE Vulkan::Vulkan)
    # Don't use system Vulkan headers - use MapLibre's vendored headers for compatibility
    # The vendored headers are included via the Mapbox::Map target
endif()

if(MLN_WITH_OPENGL AND MLN_WITH_EGL)
    if(WIN32)
        # On Windows, use ANGLE libraries from vcpkg
        find_package(unofficial-angle CONFIG REQUIRED)
        target_link_libraries(maplibre-jni PRIVATE unofficial::angle::libEGL unofficial::angle::libGLESv2)
    else()
        # On Linux, use system EGL
        find_library(EGL_LIBRARY EGL)
        if(NOT EGL_LIBRARY)
            message(FATAL_ERROR "EGL library not found")
        endif()
        target_link_libraries(maplibre-jni PRIVATE ${EGL_LIBRARY})
    endif()
elseif(MLN_WITH_OPENGL)
    find_package(OpenGL REQUIRED)
    target_link_libraries(maplibre-jni PRIVATE OpenGL::GL)
    target_include_directories(maplibre-jni SYSTEM PRIVATE ${OpenGL_INCLUDE_DIR})
    
    # Link GLX on Linux when not using EGL
    if(UNIX AND NOT APPLE AND NOT MLN_WITH_EGL)
        find_package(OpenGL REQUIRED COMPONENTS GLX)
        target_link_libraries(maplibre-jni PRIVATE OpenGL::GLX)
    endif()
endif()


# Set output directory
set_target_properties(maplibre-jni PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build/lib/main/shared
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build/lib/main/shared
)
