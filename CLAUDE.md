# MapLibre Native JVM Integration Project

## Overview
This project creates JVM bindings for MapLibre Native using direct C++ JNI, enabling MapLibre to render in Java AWT/Swing and ultimately in Compose Multiplatform Desktop.

## Architecture
- **MapLibre Native**: C++ map rendering engine
- **C++ JNI Layer**: Direct JNI bindings to MapLibre types
- **Kotlin API**: High-level API for Java/Kotlin developers with automatic memory management
- **Platform-specific Backends**: Metal on macOS, OpenGL ES via EGL on Linux/Windows

## Build System
Uses CMake to build MapLibre from source with custom JNI code:
- **MapLibre Native as Git Submodule**: At `vendor/maplibre-native`
- **CMake Integration**: Single CMakeLists.txt builds both MapLibre and JNI code

## Current Status (2025-08-03)

### What Works Now
- ✅ **Complete rendering pipeline**: Map → Frontend → Backend → Native API → Display
- ✅ **Native Metal backend on macOS**: Direct Metal rendering without ANGLE translation layer
- ✅ **OpenGL ES backend on Linux/Windows**: Using EGL for context management
- ✅ **Cross-platform JAWT integration**: Unified JAWTRendererBackend with platform-specific implementations
- ✅ **Network resource loading**: Remote styles and tiles load successfully
- ✅ **Async event processing**: RunLoop processes callbacks in render loop
- ✅ **MapLibre initialization**: All components initialize successfully
- ✅ **Map observer callbacks**: All events fire correctly (style loaded, map loaded, etc.)
- ✅ **Static rendering**: Map renders correctly after window resize

### What Isn't Yet Implemented
- ❌ **User interaction**: No mouse/keyboard controls for pan/zoom/rotate
- ❌ **Memory management**: Cleanup in `dispose()` is disabled to avoid crashes
- ❌ **Runtime styling**: Cannot modify or change styles at runtime
- ❌ **Offline maps**: No support for offline tiles or caching yet
- ❌ **Error handling**: No robust error handling or logging implemented

### Known Rendering Issues
- ⚠️ **Initial render**: Map doesn't render until first window resize
- ⚠️ **Pixel ratio**: Scale factor not correctly applied on initial render
- ⚠️ **Render strategy unclear**: Continuous 60fps rendering vs on-demand rendering when map is dirty

### Architecture Status
```
✅ Java AWT Canvas
    ↓
✅ JAWT (Native window handle extraction)
    ↓
✅ Platform Backend (Metal on macOS, EGL/OpenGL ES on Linux/Windows)
    ↓
✅ RunLoop (processes async events)
    ↓
✅ JAWTRendererBackend (C++, ContextMode::Unique)
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
1. **User Interaction**: Add mouse/keyboard controls for pan, zoom, rotate
2. **Performance Optimization**: Profile and optimize render loop
3. **Memory Management**: Fix cleanup issues in dispose()
4. **Additional Features**: 
   - Custom layers and sources
   - Offline map support
   - Style switching at runtime

### Immediate Issues to Fix
1. **Initial render bug**: Map doesn't display until window is resized
2. **Pixel ratio bug**: Initial scale factor not applied correctly
3. **Render loop inefficiency**: Currently rendering at 60fps continuously, unclear if necessary
4. **Memory cleanup**: Disabled to avoid crashes (needs investigation)
5. **No user interaction**: Mouse/keyboard controls not implemented

## Technical Implementation Details

### Critical Implementation Discoveries

#### OpenGL ES Compatibility
- **Context Type**: MapLibre expects OpenGL ES, not desktop OpenGL
- **Context Mode**: `ContextMode::Unique` required for proper state management
- **Framebuffer**: Explicit RGBA8/stencil8/depth16 configuration needed
- **Render Order**: Let MapLibre handle clearing, don't override with manual clears

#### Native Metal Integration (2025-08-03)
- **Native Metal Backend**: Replaced ANGLE with direct Metal rendering on macOS
- **Platform-specific Backends**: Metal on macOS, OpenGL ES via EGL on Linux/Windows
- **JAWT Version 9**: Required for modern Java (version constant: 0x00090000)
- **CALayer Assignment**: Direct assignment of Metal layer to JAWT surface layers
- **GLFW Pattern**: Metal backend mimics GLFW implementation exactly for consistency

#### Rendering Issues and Fixes
- **Remove canvas.repaint()**: AWT's repaint was causing buffer clearing and flickering
- **Single render path**: Only render in timer loop, not in observer callbacks
- **Initial render bug**: Map doesn't display until window resize (needs investigation)
- **Scale factor bug**: Pixel ratio not applied correctly on initial render

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
  - **Linux**: Mesa EGL/OpenGL ES or ANGLE
  - **Windows**: ANGLE libraries for D3D11 backend

## Project Structure
```
maplibre-jni/
├── CMakeLists.txt              # CMake build configuration with platform conditionals
├── build.gradle.kts            # Gradle with CMake integration
└── src/main/cpp/
    ├── jni_helpers.hpp         # Common JNI utilities
    ├── jni_*_types.cpp         # Core type wrappers
    ├── jni_jawt_backend.hpp    # Base JAWT backend interface
    ├── jni_metal_backend.mm    # Metal backend for macOS
    ├── jni_gl_backend.cpp      # OpenGL ES backend for Linux/Windows
    ├── jni_renderer_frontend_impl.cpp  # Renderer frontend
    └── jni_maplibre_map.cpp    # Map and observer wrappers

src/main/kotlin/
└── com/maplibre/jni/           # Kotlin API layer
    ├── NativeObject.kt         # Base class with automatic cleanup
    ├── *Types.kt               # MapLibre type wrappers
    ├── MapObserver.kt          # Event callbacks
    ├── JAWTRendererBackend.kt  # Platform-agnostic backend wrapper
    └── MaplibreMap.kt          # Main map API

src/main/kotlin/Main.kt         # Demo application
```

## References
- MapLibre Native: https://github.com/maplibre/maplibre-native
- GLFW platform example: https://github.com/maplibre/maplibre-native/tree/main/platform/glfw
- MapLibre demo tiles: https://demotiles.maplibre.org/

## Memories and Notes

### Build and Development Notes
- Never do a ./gradlew clean. This requires rebuilding maplibre native from scratch, which takes a long time.

## Implementation Decision: Native Metal Backend (2025-08-03)

### Decision
Dropped ANGLE dependency on macOS in favor of native Metal backend, while keeping OpenGL ES via EGL for Linux/Windows.

### Implementation Details
- **macOS**: Direct Metal backend mimicking GLFW implementation
- **Windows**: OpenGL ES via ANGLE's D3D11 backend and EGL
- **Linux**: System Mesa EGL or ANGLE's OpenGL backend
- **JAWT Integration**: Platform-specific backend selection at compile time
- **Factory Pattern**: `createPlatformBackend()` creates appropriate backend per platform

### Benefits Achieved
- Native performance on macOS without translation overhead
- Eliminated ANGLE dependency on macOS
- Direct Metal API access for future optimizations
- Cleaner architecture with platform-specific implementations
- Better alignment with MapLibre's native platform support

### Lessons Learned
- GLFW Metal backend pattern works perfectly for JAWT integration
- MetalRenderableResource must be in mbgl namespace (exact GLFW structure)
- CAMetalLayer assignment to JAWT surface layers works seamlessly
- Initial render and scale issues need further investigation

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

## Original macOS Support Research (2025-08-02)

### Problem
OpenGL ES is not available on macOS, requiring an alternative rendering approach.

### Option 1: ANGLE (OpenGL ES → Metal)

**Overview**: Use Google's ANGLE library to translate OpenGL ES calls to Metal

**Pros:**
- Minimal code changes - keep existing OpenGL ES renderer backend
- ANGLE has mature Metal backend (used by Chrome/Firefox on macOS)
- Existing Java bindings available (JANGLE project)
- Works with current JOGL-based architecture
- Drop-in replacement for OpenGL ES context

**Cons:**
- Additional dependency and translation layer overhead
- Requires building/bundling ANGLE libraries
- Performance overhead from API translation
- Less control over Metal optimizations
- Another layer of abstraction to debug

**Implementation Steps:**
1. Build ANGLE with Metal backend for macOS
2. Replace JOGL's GL context with ANGLE's EGL context
3. Use JANGLE or create JNI bindings for ANGLE's EGL
4. Minimal changes to existing renderer backend code

### Option 2: Vulkan/MoltenVK (Recommended)

**Overview**: Use MapLibre's Vulkan renderer with MoltenVK for Metal translation

**Pros:**
- MapLibre already has complete, production-ready Vulkan renderer backend
- Better performance (direct to Metal via MoltenVK)
- More future-proof (Vulkan is actively developed)
- Existing Java bindings available (LWJGL3-AWT)
- Cross-platform consistency (same renderer on all platforms)
- MoltenVK is officially supported by Khronos Group

**Cons:**
- Requires replacing OpenGL ES backend with Vulkan
- Need to handle JAWT surface creation differently
- More significant code changes required
- Learning curve for Vulkan if unfamiliar

**Implementation Steps:**
1. Enable `MLN_WITH_VULKAN` in CMake, disable `MLN_WITH_OPENGL`
2. Create JNI Vulkan backend similar to existing JOGL backend
3. Use VkBootstrap for simplified Vulkan initialization
4. Create Metal surface via MoltenVK and JAWT
5. Option: Use LWJGL3-AWT for Java Vulkan bindings (or custom JNI)

### Recommendation: Vulkan/MoltenVK Approach

**Reasoning:**
1. MapLibre's Vulkan backend is production-ready and actively maintained
2. Better long-term maintainability (single renderer path)
3. Superior performance characteristics
4. Aligns with industry direction (Apple deprecated OpenGL)
5. Vulkan provides more control and debugging capabilities
6. MoltenVK is transparent - no code changes needed for macOS

**Key Implementation Details:**
- Use VkBootstrap to simplify Vulkan initialization (removes ~400 lines of boilerplate)
- MoltenVK handles Vulkan→Metal translation transparently
- JAWT provides native window handle for surface creation
- Can leverage existing GLFW Vulkan backend as reference implementation
- Consider LWJGL3-AWT's AWTVKCanvas for simpler integration
