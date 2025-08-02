cmake_minimum_required(VERSION 3.21)
project(maplibre-jni)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Objective-C++ on macOS
if(APPLE)
    enable_language(OBJCXX)
endif()

# Find JNI
find_package(JNI REQUIRED)

# Download and extract ANGLE for macOS
if(APPLE)
    include(FetchContent)
    FetchContent_Declare(
        angle_macos
        URL https://github.com/kivy/angle-builder/releases/download/chromium-7151_rev1/angle-macos-universal.tar.gz
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    FetchContent_MakeAvailable(angle_macos)
    
    # Set ANGLE paths
    set(ANGLE_ROOT ${angle_macos_SOURCE_DIR})
    set(ANGLE_INCLUDE_DIR ${ANGLE_ROOT}/include)
    set(ANGLE_LIB_DIR ${ANGLE_ROOT})  # Libraries are in root, not lib subdirectory
endif()

# Enable position-independent code
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Configure MapLibre Native build options
set(MLN_WITH_OPENGL ON CACHE BOOL "" FORCE)
set(MLN_WITH_RTTI ON CACHE BOOL "" FORCE)
set(MLN_WITH_WERROR OFF CACHE BOOL "" FORCE)
set(MLN_WITH_GLFW OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)

# Add MapLibre Native
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../vendor/maplibre-native maplibre-native)

# Create our JNI library
set(MAPLIBRE_JNI_SOURCES
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
    src/main/cpp/jni_egl_backend.cpp
    src/main/cpp/jni_egl_renderer_backend.cpp
    src/main/cpp/jni_renderer_frontend.cpp
    src/main/cpp/jni_renderer_frontend_impl.cpp
    src/main/cpp/jni_gl_functions.cpp
)

# Add macOS-specific Objective-C++ source
if(APPLE)
    list(APPEND MAPLIBRE_JNI_SOURCES src/main/cpp/jni_egl_backend_macos.mm)
endif()

add_library(maplibre-jni SHARED ${MAPLIBRE_JNI_SOURCES})

# Include directories
target_include_directories(maplibre-jni PRIVATE
    ${JNI_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/maplibre-native/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/maplibre-native/platform/default/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/maplibre-native/src
)

# Add ANGLE include directory for EGL headers
if(APPLE)
    target_include_directories(maplibre-jni PRIVATE ${ANGLE_INCLUDE_DIR})
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
elseif(WIN32)
    target_link_libraries(maplibre-jni PRIVATE ${JAVA_HOME}/lib/jawt.lib)
else()
    # Linux
    find_library(JAWT_LIBRARY jawt HINTS ${JAVA_HOME}/lib ${JAVA_HOME}/jre/lib/amd64)
    if(JAWT_LIBRARY)
        target_link_libraries(maplibre-jni PRIVATE ${JAWT_LIBRARY})
    endif()
endif()

# Platform-specific EGL/ANGLE linking
if(APPLE)
    # macOS: Use downloaded ANGLE libraries
    target_link_libraries(maplibre-jni PRIVATE
        ${ANGLE_LIB_DIR}/libEGL.dylib
        ${ANGLE_LIB_DIR}/libGLESv2.dylib
    )
    # Copy ANGLE libraries to output directory
    add_custom_command(TARGET maplibre-jni POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${ANGLE_LIB_DIR}/libEGL.dylib
            ${ANGLE_LIB_DIR}/libGLESv2.dylib
            $<TARGET_FILE_DIR:maplibre-jni>
    )
    # Set rpath for finding the libraries at runtime
    set_target_properties(maplibre-jni PROPERTIES
        INSTALL_RPATH "@loader_path"
        BUILD_WITH_INSTALL_RPATH TRUE
    )
elseif(WIN32)
    # Windows: Use bundled ANGLE libraries
    target_link_libraries(maplibre-jni PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../libs/windows/libEGL.lib
        ${CMAKE_CURRENT_SOURCE_DIR}/../libs/windows/libGLESv2.lib
    )
else()
    # Linux: Try system EGL first, fallback to bundled ANGLE
    find_package(PkgConfig)
    if(PkgConfig_FOUND)
        pkg_check_modules(EGL egl)
        pkg_check_modules(GLES2 glesv2)
    endif()
    
    if(EGL_FOUND AND GLES2_FOUND)
        target_link_libraries(maplibre-jni PRIVATE ${EGL_LIBRARIES} ${GLES2_LIBRARIES})
        target_include_directories(maplibre-jni PRIVATE ${EGL_INCLUDE_DIRS} ${GLES2_INCLUDE_DIRS})
    else()
        # Use bundled ANGLE
        target_link_libraries(maplibre-jni PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/../libs/linux/libEGL.so
            ${CMAKE_CURRENT_SOURCE_DIR}/../libs/linux/libGLESv2.so
        )
    endif()
endif()

# Set output directory
set_target_properties(maplibre-jni PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build/lib/main/shared
)
