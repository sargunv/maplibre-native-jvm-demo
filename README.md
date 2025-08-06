# kotlin-maplibre-native

MapLibre Native JVM bindings for desktop platforms. This project enables MapLibre Native rendering in Java/Kotlin desktop applications using AWT/Swing with native graphics backends (Metal on macOS, OpenGL/Vulkan on Linux, OpenGL on Windows).

### Platform Support

| Renderer      | EGL | WGL | Vulkan | Metal |
| ------------- | --- | --- | ------ | ----- |
| macOS arm64   | ❌   | N/A | ❌      | ✅     |
| macOS x64     | ❌   | N/A | ❌      | ❔*    |
| Linux arm64   | ✅   | N/A | ✅      | N/A   |
| Linux x64     | ✅   | N/A | ✅      | N/A   |
| Windows arm64 | ✅   | N/A | ❌      | N/A   |
| Windows x64   | ✅   | ✅   | ❌      | N/A   |

* untested

### What Works
- Complete rendering pipeline with MapLibre Native integration
- Network resource loading (remote styles and tiles)
- Map observer callbacks and event handling
- User interaction (pan, zoom, rotate via mouse/keyboard)

### What Doesn't Work Yet
- Runtime styling
- Offline maps
- A bunch of other misc API methods
