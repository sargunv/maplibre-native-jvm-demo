# MapLibre Native JVM Integration Project

## Overview
This project creates JVM bindings for MapLibre Native using direct C++ JNI, enabling MapLibre to render in Java AWT/Swing and ultimately in Compose Multiplatform Desktop.

## Architecture
- **MapLibre Native**: C++ map rendering engine
- **C++ JNI Layer**: Direct JNI bindings to MapLibre types
- **Kotlin API**: High-level API for Java/Kotlin developers with automatic memory management
- **Platform-specific Backends**: Metal on macOS, Vulkan on Linux/Windows (in development)

## Build System
Uses CMake to build MapLibre from source with custom JNI code:
- **MapLibre Native as Git Submodule**: At `vendor/maplibre-native`
- **CMake Integration**: Single CMakeLists.txt builds both MapLibre and JNI code

## Current Status (2025-08-04)

### What Works Now
- ✅ **Complete rendering pipeline**: Map → Frontend → Backend → Native API → Display
- ✅ **Native Metal backend on macOS**: Direct Metal rendering without translation layers
- ✅ **Cross-platform JAWT integration**: Unified AwtCanvasRenderer with platform-specific backends
- ✅ **Native Vulkan backend on Linux**: Direct Vulkan rendering via X11 surface
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
- ❌ **Windows support**: Vulkan backend needs Windows-specific surface creation

### Known Issues
- **Vulkan resize flickering**: The Vulkan backend (Linux) exhibits flickering during window resize. This is due to swapchain recreation timing - the renderer continues using the old swapchain while the window has already resized. The Metal backend (macOS) does not have this issue.

### Architecture Status
```
✅ Java AWT Canvas
    ↓
✅ JAWT (Native window handle extraction)
    ↓
✅ Platform Backend (Metal on macOS, Vulkan on Linux)
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
1. **Windows support**: Add Windows surface creation for Vulkan backend
2. **Linux/Vulkan resize flickering**: Investigate swapchain recreation timing to eliminate flicker

#### Native Metal Integration (2025-08-03)
- **Native Metal Backend**: Direct Metal rendering on macOS
- **Platform-specific Backends**: Metal on macOS, Vulkan on Linux (Windows planned)
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

## Build Requirements
- C++ compiler with C++20 support
- Java Development Kit (JDK 11+)
- CMake 3.21+
- Platform-specific:
  - **macOS**: Metal framework (included with Xcode)
  - **Linux**: Vulkan SDK and drivers
  - **Windows**: Vulkan SDK (not yet implemented)

## Project Structure
```
maplibre-jni/
├── CMakeLists.txt              # CMake build configuration with platform conditionals
├── build.gradle.kts            # Gradle with CMake integration
└── src/main/cpp/
    ├── jni_helpers.hpp         # Common JNI utilities
    ├── awt_metal_backend.mm    # Metal backend for macOS
    ├── awt_vulkan_backend.cpp  # Vulkan backend for Linux/Windows
    ├── awt_backend_factory.cpp # Platform backend factory
    ├── awt_canvas_renderer.cpp # Unified renderer frontend
    └── maplibre_map.cpp    # Map and observer wrappers

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
- **Linux**: Vulkan backend (implemented)
- **Windows**: Vulkan backend (surface creation not yet implemented)

### Implementation Details
- **No external graphics libraries**: No ANGLE, JOGL, or EGL dependencies
- **Native APIs only**: Direct Metal on macOS, Vulkan on Linux/Windows (planned)
- **JAWT Integration**: Direct native window handle extraction for all platforms
- **Factory Pattern**: `createPlatformBackend()` creates appropriate backend per platform

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

## Vulkan Backend Implementation Notes (2025-08-04)

### Implementation Status
Successfully implemented native Vulkan backend for Linux:
1. X11 surface creation via JAWT
2. Proper swapchain management
3. Integration with MapLibre's Vulkan renderer
4. Follows GLFW Vulkan backend patterns

### Key Technical Details
- Vulkan backend inherits from `mbgl::vulkan::RendererBackend` and `mbgl::vulkan::Renderable`
- VulkanRenderableResource in global namespace (matching GLFW pattern)
- X11 surface created from JAWT drawable
- Immediate JAWT surface release after handle extraction (prevents AWT blocking)
- Calls `requestSurfaceUpdate()` on resize for swapchain recreation

### Known Issues
- **Resize flickering**: Swapchain recreation has inherent latency causing visible flicker during resize
- **Windows support**: Need to add Win32 surface creation

