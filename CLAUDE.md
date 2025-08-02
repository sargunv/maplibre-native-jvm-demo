# MapLibre Native JVM Integration Project

## Overview
This project creates JVM bindings for MapLibre Native using direct C++ JNI, enabling MapLibre to render in Java AWT/Swing and ultimately in Compose Multiplatform Desktop.

## Architecture
- **MapLibre Native**: C++ map rendering engine
- **C++ JNI Layer**: Direct JNI bindings to MapLibre types
- **Kotlin API**: High-level API for Java/Kotlin developers with automatic memory management
- **JOGL Integration**: OpenGL ES context management for rendering

## Build System
Uses CMake to build MapLibre from source with custom JNI code:
- **MapLibre Native as Git Submodule**: At `vendor/maplibre-native`
- **CMake Integration**: Single CMakeLists.txt builds both MapLibre and JNI code

## Current Status (2025-08-01)

### What Works Now
- ✅ **Complete rendering pipeline**: Map → Frontend → Backend → JOGL → Display
- ✅ **Network resource loading**: Remote styles and tiles load successfully
- ✅ **Async event processing**: RunLoop processes callbacks in render loop
- ✅ **MapLibre initialization**: All components initialize successfully
- ✅ **OpenGL ES compatibility**: Proper OpenGL ES 3.2 context with framebuffer config
- ✅ **Map observer callbacks**: All events fire correctly (style loaded, map loaded, etc.)

### What Isn't Yet Implemented
- ❌ **User interaction**: No mouse/keyboard controls for pan/zoom/rotate
- ❌ **Memory management**: Cleanup in `dispose()` is disabled to avoid crashes
- ❌ **Runtime styling**: Cannot modify or change styles at runtime
- ❌ **Offline maps**: No support for offline tiles or caching yet
- ❌ **Error handling**: No robust error handling or logging implemented

### Architecture Status
```
✅ JOGL GLJPanel (OpenGL ES 3.2)
    ↓
✅ RunLoop (processes async events)
    ↓
✅ JOGLRendererBackend (C++, ContextMode::Unique)
    ↓
✅ JOGLRendererFrontend (C++)
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

### Known Issues
- Memory cleanup disabled to avoid crashes (needs investigation)
- No user interaction implemented yet

## Technical Implementation Details

### OpenGL ES Compatibility Fixes
**Critical discoveries** from GLFW demo comparison:
- **Context Type**: MapLibre expects OpenGL ES, not desktop OpenGL
- **Context Mode**: `ContextMode::Unique` required for proper state management
- **Framebuffer**: Explicit RGBA8/stencil8/depth16 configuration needed
- **Render Order**: Let MapLibre handle clearing, don't override with manual clears

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
- C++ compiler with C++17 support
- Java Development Kit (JDK 11+)
- JOGL (Java OpenGL bindings)
- CMake 3.20+
- OpenGL ES 3.2 support

## Project Structure
```
maplibre-jni/
├── CMakeLists.txt              # CMake build configuration
├── build.gradle.kts            # Gradle with CMake integration
└── src/main/cpp/
    ├── jni_helpers.hpp         # Common JNI utilities
    ├── jni_*_types.cpp         # Core type wrappers
    ├── jni_jogl_backend.cpp    # JOGL OpenGL ES backend
    ├── jni_jogl_renderer_frontend_impl.cpp  # Renderer frontend
    └── jni_map_wrapper.cpp     # Map and observer wrappers

src/main/kotlin/
└── com/maplibre/jni/           # Kotlin API layer
    ├── NativeObject.kt         # Base class with automatic cleanup
    ├── *Types.kt               # MapLibre type wrappers
    ├── MapObserver.kt          # Event callbacks
    └── MaplibreMap.kt          # Main map API
, with a focus on simplicity and ease of use.
src/main/kotlin/Main.kt         # Demo application
```

## References
- MapLibre Native: https://github.com/maplibre/maplibre-native
- GLFW platform example: https://github.com/maplibre/maplibre-native/tree/main/platform/glfw
- MapLibre demo tiles: https://demotiles.maplibre.org/

## Memories and Notes

### Build and Development Notes
- Never do a ./gradlew clean. This requires rebuilding maplibre native from scratch, which takes a long time.

## macOS Support Research (2025-08-02)

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
