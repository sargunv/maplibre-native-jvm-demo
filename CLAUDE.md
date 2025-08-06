# MapLibre Native JVM Integration Project

## Overview
This project creates JVM bindings for MapLibre Native using direct C++ JNI, enabling MapLibre to render in Java AWT/Swing and ultimately in Compose Multiplatform Desktop.

## Architecture
- **MapLibre Native**: C++ map rendering engine
- **C++ JNI Layer**: Direct JNI bindings to MapLibre types
- **Kotlin API**: High-level API for Java/Kotlin developers with automatic memory management
- **Platform-specific Backends**: Metal on macOS, OpenGL/Vulkan on Linux/Windows

## Build System
Uses CMake to build MapLibre from source with custom JNI code:
- **MapLibre Native as Git Submodule**: At `vendor/maplibre-native`
- **CMake Integration**: Single CMakeLists.txt builds both MapLibre and JNI code

## Current Status (2025-01-06)

### What Works Now
- ✅ **Complete rendering pipeline**: Map → Frontend → Backend → Native API → Display
- ✅ **Native Metal backend on macOS**: Direct Metal rendering without translation layers
- ✅ **Native OpenGL backend on Linux**: OpenGL ES 2.0 rendering via EGL
- ✅ **Native Vulkan backend on Linux**: Direct Vulkan rendering via X11 surface (optional)
- ✅ **Native OpenGL backend on Windows x64**: OpenGL 3.0 rendering via WGL
- ✅ **Native OpenGL backend on Windows ARM64**: OpenGL ES 2.0 rendering via EGL/ANGLE
- ✅ **Network resource loading**: Remote styles and tiles load successfully
- ✅ **Async event processing**: RunLoop processes callbacks in render loop
- ✅ **MapLibre initialization**: All components initialize successfully
- ✅ **Map observer callbacks**: All events fire correctly (style loaded, map loaded, etc.)
- ✅ **Map rendering**: Map renders correctly with proper colors and no initial render bug
- ✅ **User interaction**: Mouse/keyboard controls for pan/zoom/rotate

### What Isn't Yet Implemented
- ❌ **Runtime styling**: Cannot modify or change styles at runtime
- ❌ **Offline maps**: No support for offline tiles or caching yet
- ❌ **Error handling**: No robust error handling or logging implemented

### Known Issues
- Application crashes on exit with Windows ARM64 EGL backend (exit code -1073741819)

### Architecture Status
```
✅ Java AWT Canvas
    ↓
✅ JAWT (Native window handle extraction)
    ↓
✅ Platform Backend (Metal on macOS, OpenGL/Vulkan on Linux, OpenGL on Windows)
    ↓
✅ RunLoop (processes async events)
    ↓
✅ AwtCanvasRenderer (C++, unified frontend)
    ↓
✅ RendererFrontend (C++)
    ↓
✅ Map (C++) ← MapObserver (C++→JNI→Kotlin)
    ↓
✅ File Sources (Network, ResourceLoader, Database)
    ↓
✅ Kotlin API Layer
```

## Next Steps & Future Improvements

### Potential Enhancements
2. **Performance Optimization**: Profile and optimize render loop
3. **Memory Management**: Fix cleanup issues in dispose()
4. **Additional Features**:
   - Custom layers and sources
   - Offline map support
   - Style switching at runtime

### Immediate Issues to Fix
- None currently

#### Native Metal Integration (2025-08-03)
- **Native Metal Backend**: Direct Metal rendering on macOS
- **Platform-specific Backends**: Metal on macOS, OpenGL/Vulkan on Linux, OpenGL on Windows
- **JAWT Version 9**: Required for modern Java (version constant: 0x00090000)
- **CALayer Configuration**: Must set frame, opaque, contentsScale, and pixelFormat properties
- **GLFW Pattern**: Metal backend mimics GLFW implementation exactly for consistency

#### Rendering Issues and Fixes
- **Initial render fix**: Setting Metal layer frame resolves the initial render bug
- **Color accuracy**: Use `MTLPixelFormatBGRA8Unorm` instead of sRGB variant
- **Single render path**: Only render in timer loop, not in observer callbacks
- **Remove canvas.repaint()**: AWT's repaint was causing buffer clearing and flickering

### JNI Implementation Pattern
```cpp
// Common helpers in jni_helpers.hpp
template<typename T>
inline T* fromJavaPointer(jlong ptr) {
    return reinterpret_cast<T*>(ptr);
}

template<typename T>
inline jlong toJavaPointer(T* ptr) {
    return reinterpret_cast<jlong>(ptr);
}
```

### Kotlin API Pattern
```kotlin
open class NativeObject internal constructor(
    new: () -> Long,
    destroy: (Long) -> Unit
) : AutoCloseable {
    protected val nativePtr: Long = new()
    private val cleanable = cleaner.register(this) { destroy(nativePtr) }
    override fun close() { cleanable.clean() }
}
```

## Project Structure
```
maplibre-jni/
├── CMakeLists.txt              # CMake build configuration with platform conditionals
├── build.gradle.kts            # Gradle with CMake integration
└── src/main/cpp/
    ├── jni_helpers.hpp         # Common JNI utilities
    ├── awt_metal_backend.mm    # Metal backend for macOS
    ├── awt_wgl_backend.cpp     # WGL backend for Windows x64
    ├── awt_egl_backend.cpp     # EGL backend for Linux and Windows ARM64
    ├── awt_vulkan_backend.cpp  # Vulkan backend (optional)
    ├── awt_backend_factory.cpp # Platform backend factory
    ├── awt_canvas_renderer.cpp # Unified renderer frontend
    └── maplibre_map.cpp        # Map and observer wrappers

src/main/kotlin/
└── com/maplibre/jni/           # Kotlin API layer
    ├── NativeObject.kt         # Base class with automatic cleanup
    ├── *Types.kt               # MapLibre type wrappers
    ├── MapObserver.kt          # Event callbacks
    ├── MaplibreCanvas.kt       # High-level canvas component
    └── MaplibreMap.kt          # Main map API

src/main/kotlin/Main.kt         # Demo application
```

## References
- MapLibre Native: https://github.com/maplibre/maplibre-native
- GLFW platform example: https://github.com/maplibre/maplibre-native/tree/main/platform/glfw
- MapLibre demo tiles: https://demotiles.maplibre.org/

## Memories and Notes

### Build and Development Notes
- Never do a ./gradlew clean. This requires rebuilding maplibre native from scratch, which takes a long time.\
- Similarly, never do an rm -rf /Users/sargunv/Code/maplibre-native-jvm-demo/maplibre-jni/build/cmake
- Instead, allow cmake to update the configuration files as needed.

## Implementation Decision: Native Multiplatform Backends (2025-08-03)

### Decision
Implemented native graphics backends for each platform without any translation layers:
- **macOS**: Direct Metal backend
- **Linux**: OpenGL (default) and Vulkan (optional) backends
- **Windows**: OpenGL backend via WGL

### Benefits Achieved
- Native performance without any translation overhead
- Minimal dependencies (only system graphics libraries)
- Direct API access for platform-specific optimizations
- Clean, lean MVP architecture
- Better alignment with MapLibre's native platform support

## Metal Backend Implementation Notes (2025-08-03)

### Implementation Approach
Successfully implemented native Metal backend by:
1. Studying GLFW Metal backend implementation
2. Mimicking GLFW structure exactly (namespace, class hierarchy)
3. Adapting JAWT surface layer assignment pattern from GLFW's NSWindow approach
4. Using CMake conditionals for platform-specific compilation

### Key Technical Details
- Metal backend inherits from `mbgl::mtl::RendererBackend` and `mbgl::gfx::Renderable`
- MetalRenderableResource in `mbgl` namespace (required for proper integration)
- CAMetalLayer created and assigned to JAWT surface layers
- Command queue and buffers managed per GLFW pattern
- Depth and stencil textures created with proper Metal usage flags

## OpenGL Backend Implementation Notes (2025-01-04)

### Implementation Status
Successfully implemented native OpenGL backend for Linux:
1. X11 window handle extraction via JAWT
2. EGL context creation and management
3. OpenGL ES 2.0 rendering context
4. Integration with MapLibre's GL renderer
5. Follows GLFW OpenGL backend patterns

### Key Technical Details
- OpenGL backend inherits from `mbgl::gl::RendererBackend` and `mbgl::gfx::Renderable`
- OpenGLRenderableResource implements `bind()` and `swap()` for buffer management
- EGL 1.5 for context creation and surface management
- X11 window handle extracted from JAWT drawable
- Immediate JAWT surface release after handle extraction (prevents AWT blocking)
- Double buffering via `eglSwapBuffers()`

### Build Configuration
- Default backend for Linux/Windows (no flag needed)
- Use `USE_VULKAN_BACKEND=ON` CMake flag to use Vulkan instead
- Automatically links against OpenGL and EGL libraries

## Vulkan Backend Implementation Notes (2025-01-04, Windows support 2025-01-06)

### Implementation Status
Successfully implemented native Vulkan backend for Linux and Windows:
1. Platform-specific surface creation (X11 on Linux, Win32 on Windows)
2. Proper swapchain management
3. Integration with MapLibre's Vulkan renderer
4. Follows GLFW Vulkan backend patterns

### Key Technical Details
- Vulkan backend inherits from `mbgl::vulkan::RendererBackend` and `mbgl::vulkan::Renderable`
- VulkanRenderableResource in global namespace (matching GLFW pattern)
- Platform-specific surface creation:
  - Linux: X11 surface via `vkCreateXlibSurfaceKHR`
  - Windows: Win32 surface via `vkCreateWin32SurfaceKHR`
- Immediate JAWT surface release after handle extraction (prevents AWT blocking)
- Calls `requestSurfaceUpdate()` on resize for swapchain recreation

### Build Configuration
- Use `MLN_WITH_VULKAN=ON` CMake flag or `windows-vulkan`/`linux-vulkan` presets
- Uses MapLibre's vendored Vulkan headers for API compatibility
- Links against system Vulkan library (vulkan-1.dll on Windows, libvulkan.so on Linux)


## Windows ARM64 EGL Backend Implementation Notes (2025-01-06)

### Implementation Status
Successfully implemented native EGL backend for Windows ARM64:
1. Split OpenGL backend into separate WGL and EGL implementations
2. EGL backend uses ANGLE (Almost Native Graphics Layer Engine) for OpenGL ES → DirectX translation
3. Windows ARM64 devices lack native OpenGL drivers, making ANGLE/EGL the only viable option
4. Same EGL backend works on both Linux (native EGL) and Windows (ANGLE)

### Key Technical Details
- EGL backend inherits from `mbgl::gl::RendererBackend` and `mbgl::gfx::Renderable`
- Uses `EGL_DEFAULT_DISPLAY` on Windows (ANGLE pattern)
- Creates OpenGL ES 2.0 context (`EGL_CONTEXT_CLIENT_VERSION, 2`)
- ANGLE libraries (libEGL.dll, libGLESv2.dll) provided by MapLibre's vcpkg dependencies
- Requires Visual C++ runtime (vcruntime140.dll, msvcp140.dll) and DirectX (dxgi.dll, d3d9.dll)

### Build Configuration
- Use `MLN_WITH_EGL=ON` CMake flag for EGL backend
- Default WGL backend: `windows-wgl` preset (Windows x64)
- EGL backend: `windows-egl` preset (Windows ARM64)
- All dependency DLLs automatically packaged as resources

### Native Library Loading
- Windows requires loading all dependency DLLs in correct order
- System libraries (VC++ runtime, DirectX) loaded via `System.loadLibrary()`
- Application libraries extracted to temp directory and loaded via `System.load()`
- Load order matters: dependencies → libGLESv2.dll → libEGL.dll → maplibre-jni.dll

## Windows OpenGL Backend Implementation Notes (2025-01-05)

### Implementation Status
Successfully implemented native OpenGL backend for Windows:
1. HWND and HDC extraction via JAWT
2. WGL context creation and management
3. OpenGL 3.0 rendering context using WGL_ARB_create_context
4. Integration with MapLibre's GL renderer
5. Follows MapLibre's WGL patterns

### Key Technical Details
- Uses Windows native WGL (Windows OpenGL) API
- Gets device context (HDC) from HWND rather than using JAWT's HDC
- Creates temporary context to load WGL extensions
- Creates modern OpenGL 3.0 context using wglCreateContextAttribsARB
- Uses Windows SwapBuffers() for double buffering
- Immediate JAWT surface release after handle extraction (prevents AWT blocking)

### Implementation Approach
- No legacy WGL fallback - requires WGL_ARB_create_context extension
- Follows MapLibre's headless_backend_wgl.cpp patterns
- Simplified approach since JAWT provides the window handle


