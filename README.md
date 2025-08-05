# kotlin-maplibre-native

MapLibre Native JVM bindings for desktop platforms. This project enables MapLibre Native rendering in Java/Kotlin desktop applications using AWT/Swing with native graphics backends (Metal on macOS, OpenGL/Vulkan on Linux, OpenGL on Windows).

### Platform Support
- ✅ **macOS**: Native Metal backend (fully functional)
- ✅ **Linux**: Native OpenGL backend (default, functional with resize flickering)
- ✅ **Linux**: Native Vulkan backend (optional via USE_VULKAN_BACKEND flag, same resize flickering)
- ✅ **Windows**: Native OpenGL backend (functional with resize flickering)

### What Works
- Complete rendering pipeline with MapLibre Native integration
- Network resource loading (remote styles and tiles)
- Map observer callbacks and event handling
- User interaction (pan, zoom, rotate via mouse/keyboard)

### What Doesn't Work Yet
- Runtime styling (cannot modify styles after initialization)
- Offline maps (no tile caching support)
