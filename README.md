# kotlin-maplibre-native

MapLibre Native JVM bindings for desktop platforms. This project enables MapLibre Native rendering in Java/Kotlin desktop applications using AWT/Swing with native graphics backends (Metal on macOS, OpenGL/Vulkan on Linux, OpenGL on Windows).

### Platform Support
- ✅ **macOS Metal**: default
- ✅ **Linux OpenGL**: default
- ✅ **Linux Vulkan**: optional via `USE_VULKAN_BACKEND` flag
- ✅ **Windows OpenGL**: default
- ❌ **Windows Vulkan**: TODO

### What Works
- Complete rendering pipeline with MapLibre Native integration
- Network resource loading (remote styles and tiles)
- Map observer callbacks and event handling
- User interaction (pan, zoom, rotate via mouse/keyboard)

### What Doesn't Work Yet
- Runtime styling
- Offline maps
- A bunch of other misc API methods
