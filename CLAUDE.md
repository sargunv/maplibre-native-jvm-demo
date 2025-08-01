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
