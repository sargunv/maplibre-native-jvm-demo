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

# FetchContent for downloading MapLibre prebuilt libraries
include(FetchContent)

# MapLibre version to use
set(MAPLIBRE_VERSION "core-aeaadc06b4e0614f4f243db4dce210c22dde9f9c")
set(MAPLIBRE_BASE_URL "https://github.com/maplibre/maplibre-native/releases/download/${MAPLIBRE_VERSION}")

# Download headers (common for all platforms)
FetchContent_Declare(
    maplibre_headers
    URL ${MAPLIBRE_BASE_URL}/maplibre-native-headers.tar.gz
    SOURCE_DIR ${CMAKE_BINARY_DIR}/_deps/maplibre-headers
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

# Platform-specific library selection
if(APPLE)
    if(MLN_WITH_METAL)
        set(MAPLIBRE_LIB_NAME "libmaplibre-native-core-macos-arm64-metal.a")
    else()
        message(FATAL_ERROR "Only Metal backend is supported for macOS prebuilt libraries")
    endif()
elseif(WIN32)
    if(MLN_WITH_VULKAN)
        set(MAPLIBRE_LIB_NAME "maplibre-native-core-windows-x64-vulkan.lib")
    elseif(MLN_WITH_EGL)
        set(MAPLIBRE_LIB_NAME "maplibre-native-core-windows-x64-egl.lib")
    else()
        set(MAPLIBRE_LIB_NAME "maplibre-native-core-windows-x64-opengl.lib")
    endif()
elseif(UNIX)
    # Detect architecture
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64")
        set(ARCH_SUFFIX "arm64")
    else()
        set(ARCH_SUFFIX "x64")
    endif()
    
    if(MLN_WITH_VULKAN)
        set(MAPLIBRE_LIB_NAME "libmaplibre-native-core-linux-${ARCH_SUFFIX}-vulkan.a")
    else()
        set(MAPLIBRE_LIB_NAME "libmaplibre-native-core-linux-${ARCH_SUFFIX}-opengl.a")
    endif()
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

# Download the platform-specific library
FetchContent_Declare(
    maplibre_lib
    URL ${MAPLIBRE_BASE_URL}/${MAPLIBRE_LIB_NAME}
    DOWNLOAD_NO_EXTRACT TRUE
    DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/_deps/maplibre-lib
)

# Make both available
FetchContent_MakeAvailable(maplibre_headers maplibre_lib)

# Get the downloaded library path
FetchContent_GetProperties(maplibre_lib)
set(MAPLIBRE_LIB_PATH ${CMAKE_BINARY_DIR}/_deps/maplibre-lib/${MAPLIBRE_LIB_NAME})

# Create imported target for MapLibre
add_library(Mapbox::Map STATIC IMPORTED GLOBAL)
set_target_properties(Mapbox::Map PROPERTIES
    IMPORTED_LOCATION ${MAPLIBRE_LIB_PATH}
)

# Set include directories for the imported target
target_include_directories(Mapbox::Map INTERFACE
    ${CMAKE_BINARY_DIR}/_deps/maplibre-headers/include
    ${CMAKE_BINARY_DIR}/_deps/maplibre-headers/vendor/maplibre-native-base/include
    ${CMAKE_BINARY_DIR}/_deps/maplibre-headers/vendor/maplibre-native-base/deps/geometry.hpp/include
    ${CMAKE_BINARY_DIR}/_deps/maplibre-headers/vendor/maplibre-native-base/deps/variant/include
)

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
if(MLN_WITH_METAL)
    list(APPEND MAPLIBRE_JNI_SOURCES 
        src/main/cpp/awt_metal_backend.mm
    )
elseif(MLN_WITH_VULKAN)
    list(APPEND MAPLIBRE_JNI_SOURCES 
        src/main/cpp/awt_vulkan_backend.cpp
    )
    if(APPLE)
        list(APPEND MAPLIBRE_JNI_SOURCES 
            src/main/cpp/awt_vulkan_backend_macos.mm
        )
    endif()
elseif(MLN_WITH_OPENGL)
    # Common GL backend files
    list(APPEND MAPLIBRE_JNI_SOURCES 
        src/main/cpp/awt_gl_backend.cpp
        src/main/cpp/gl_context_strategy.hpp
    )
    
    if(MLN_WITH_EGL)
        list(APPEND MAPLIBRE_JNI_SOURCES 
            src/main/cpp/egl_context_strategy.cpp
        )
    elseif(WIN32)
        list(APPEND MAPLIBRE_JNI_SOURCES 
            src/main/cpp/wgl_context_strategy.cpp
        )
    elseif(UNIX AND NOT APPLE)
        # Linux with GLX (desktop OpenGL)
        list(APPEND MAPLIBRE_JNI_SOURCES 
            src/main/cpp/glx_context_strategy.cpp
        )
    endif()
else()
    message(FATAL_ERROR "Unsupported backend configuration.")
endif()

add_library(maplibre-jni SHARED ${MAPLIBRE_JNI_SOURCES})

# Include directories
target_include_directories(maplibre-jni PRIVATE
    ${JNI_INCLUDE_DIRS}
    ${JNI_HEADERS_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp
    ${CMAKE_BINARY_DIR}/_deps/maplibre-headers/include
)

# Add Windows-specific includes
if(WIN32)
    # Windows platform headers would be needed here if available in tarball
    # Note: vcpkg dependencies would need to be handled separately if needed
endif()

# Add Metal-cpp headers for macOS
if(MLN_WITH_METAL)
    # Download metal-cpp headers separately since they're not in the tarball
    FetchContent_Declare(
        metal_cpp
        GIT_REPOSITORY https://github.com/bkaradzic/metal-cpp.git
        GIT_TAG metal-cpp_macOS15.2_iOS18.2
        SOURCE_DIR ${CMAKE_BINARY_DIR}/_deps/metal-cpp
    )
    FetchContent_MakeAvailable(metal_cpp)
    
    target_include_directories(maplibre-jni PRIVATE
        ${CMAKE_BINARY_DIR}/_deps/metal-cpp
    )
endif()

target_compile_options(maplibre-jni PRIVATE
    -frtti
    -fexceptions
)

if(WIN32)
    target_compile_definitions(maplibre-jni PRIVATE
        NOMINMAX  # Prevent min/max macro conflicts
    )
endif()

# Pass backend selection to C++ code

if(MLN_WITH_METAL)
    target_compile_definitions(maplibre-jni PRIVATE USE_METAL_BACKEND)
endif()

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
    target_include_directories(maplibre-jni PRIVATE ${X11_INCLUDE_DIR})
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
    target_include_directories(maplibre-jni PRIVATE ${OpenGL_INCLUDE_DIR})
    
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
