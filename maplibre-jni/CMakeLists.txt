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

# Backend selection option
option(USE_VULKAN_BACKEND "Use Vulkan backend instead of OpenGL on Linux/Windows" OFF)

# Configure MapLibre Native build options based on platform
if(APPLE)
    if(USE_VULKAN_BACKEND)
        # TODO support Vulkan on macOS?
        message(FATAL_ERROR "USE_VULKAN_BACKEND not yet supported on macOS.")
    endif()
    set(MLN_WITH_METAL ON CACHE BOOL "" FORCE)
    set(MLN_WITH_OPENGL OFF CACHE BOOL "" FORCE)
    set(MLN_WITH_VULKAN OFF CACHE BOOL "" FORCE)
elseif(UNIX)
    set(MLN_WITH_METAL OFF CACHE BOOL "" FORCE)
    set(MLN_WITH_X11 ON CACHE BOOL "" FORCE)
    set(MLN_WITH_WAYLAND OFF CACHE BOOL "" FORCE)
    if(USE_VULKAN_BACKEND)
        set(MLN_WITH_OPENGL OFF CACHE BOOL "" FORCE)
        set(MLN_WITH_VULKAN ON CACHE BOOL "" FORCE)
    else()
        set(MLN_WITH_OPENGL ON CACHE BOOL "" FORCE)
        set(MLN_WITH_VULKAN OFF CACHE BOOL "" FORCE)
    endif()
elseif(WIN32)
    set(MLN_WITH_METAL OFF CACHE BOOL "" FORCE)
    set(MLN_WITH_EGL OFF CACHE BOOL "" FORCE) # TODO support EGL on Windows?
    if(USE_VULKAN_BACKEND)
        set(MLN_WITH_OPENGL OFF CACHE BOOL "" FORCE)
        set(MLN_WITH_VULKAN ON CACHE BOOL "" FORCE)
    else()
        set(MLN_WITH_OPENGL ON CACHE BOOL "" FORCE)
        set(MLN_WITH_VULKAN OFF CACHE BOOL "" FORCE)
    endif()
endif()

set(MLN_WITH_RTTI ON CACHE BOOL "" FORCE)
set(MLN_WITH_WERROR OFF CACHE BOOL "" FORCE)
set(MLN_WITH_GLFW OFF CACHE BOOL "" FORCE)

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
    src/main/cpp/awt_canvas_renderer.cpp
    src/main/cpp/awt_backend_factory.cpp
)

# Add platform-specific backend sources
if(APPLE)
    list(APPEND MAPLIBRE_JNI_SOURCES 
        src/main/cpp/awt_metal_backend.mm
    )
elseif(USE_VULKAN_BACKEND)
    list(APPEND MAPLIBRE_JNI_SOURCES 
        src/main/cpp/awt_vulkan_backend.cpp
    )
else()
    list(APPEND MAPLIBRE_JNI_SOURCES 
        src/main/cpp/awt_opengl_backend.cpp
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

# Add Windows-specific includes
if(WIN32)
    target_include_directories(maplibre-jni PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/maplibre-native/platform/windows/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/maplibre-native/platform/windows/vendor/vcpkg/installed/x64-windows/include
    )
endif()

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

# Windows-specific definitions
if(WIN32)
    target_compile_definitions(maplibre-jni PRIVATE
        NOMINMAX  # Prevent min/max macro conflicts
    )
endif()

# Pass backend selection to C++ code
if(USE_VULKAN_BACKEND)
    target_compile_definitions(maplibre-jni PRIVATE USE_VULKAN_BACKEND)
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
        "-framework Metal"
    )
else()
    # Find X11 for window system integration (Linux)
    if(UNIX)
        find_package(X11 REQUIRED)
        target_link_libraries(maplibre-jni PRIVATE ${X11_LIBRARIES})
        target_include_directories(maplibre-jni PRIVATE ${X11_INCLUDE_DIR})
    endif()
    
    if(USE_VULKAN_BACKEND)
        # Find Vulkan
        find_package(Vulkan REQUIRED)
        
        target_link_libraries(maplibre-jni PRIVATE Vulkan::Vulkan)
        target_include_directories(maplibre-jni PRIVATE ${Vulkan_INCLUDE_DIRS})
    else()
        # Find OpenGL
        find_package(OpenGL REQUIRED)
        
        # Find EGL - required for OpenGL context creation on Linux
        # Windows uses WGL which comes with OpenGL
        if(UNIX)
            find_library(EGL_LIBRARY EGL)
            if(NOT EGL_LIBRARY)
                message(FATAL_ERROR "EGL library not found")
            endif()
        endif()
        
        target_link_libraries(maplibre-jni PRIVATE OpenGL::GL)
        
        # Link EGL only on Linux
        if(UNIX)
            target_link_libraries(maplibre-jni PRIVATE ${EGL_LIBRARY})
        endif()
        
        # Add OpenGL/EGL include directories if needed
        if(OpenGL_INCLUDE_DIR)
            target_include_directories(maplibre-jni PRIVATE ${OpenGL_INCLUDE_DIR})
        endif()
    endif()
endif()

# Set output directory
set_target_properties(maplibre-jni PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build/lib/main/shared
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build/lib/main/shared
)
