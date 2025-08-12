# maplibre-native-jvm-demo

This project demos MapLibre Native rendering in Kotlin/JVM desktop applications using AWT with native graphics backends.

### Platform support

❌ = Not implemented<br />
✅ = Implemented and working<br />
❔ = Implemented but not tested <br />
⚠️ = Implemented but has issues (see notes below)<br />

| Renderer        | OpenGL ES     | OpenGL | Vulkan        | Metal |
| --------------- | ------------- | ------ | ------------- | ----- |
| macOS arm64     | ❌<sup>†</sup> | N/A    | ✅<sup>‡</sup> | ✅     |
| macOS x64       | ❌<sup>†</sup> | N/A    | ❔<sup>‡</sup> | ❔     |
| Linux X11 arm64 | ✅             | ✅      | ✅             | N/A   |
| Linux X11 x64   | ✅             | ✅      | ✅             | N/A   |
| Windows arm64   | ⚠️<sup>†</sup> | ❔      | ✅             | N/A   |
| Windows x64     | ✅<sup>†</sup> | ✅      | ✅             | N/A   |

<sup>†</sup>OpenGL ES on non-Linux platforms uses ANGLE<br />
<sup>‡</sup>Vulkan on macOS uses MoltenVK<br />

### Known issues

- On Windows arm64 with OpenGL ES (ANGLE), the app crashes shortly after startup (the map does render though!)

### What works
- Complete rendering pipeline with MapLibre Native integration
- Network resource loading (remote styles and tiles)
- Map observer callbacks and event handling
- User interaction (pan, zoom, rotate via mouse/keyboard)

### What isn't implemented yet
- Runtime styling
- Offline maps
- A bunch of other misc API methods

### Building with different backends

The project supports multiple graphics backends per platform. Use CMake presets to select the backend:

**macOS:**
- `./gradlew run -Pcmake.preset=macos-metal` (default) - Native Metal backend
- `./gradlew run -Pcmake.preset=macos-vulkan` - Vulkan via MoltenVK

**Linux:**
- `./gradlew run -Pcmake.preset=linux-egl` (default) - OpenGL ES 3.0 via EGL
- `./gradlew run -Pcmake.preset=linux-glx` - Desktop OpenGL 3.0 via GLX
- `./gradlew run -Pcmake.preset=linux-vulkan` - Native Vulkan

**Windows:**
- `./gradlew run -Pcmake.preset=windows-wgl` (default on x64) - OpenGL 3.0 via WGL
- `./gradlew run -Pcmake.preset=windows-vulkan` (default on ARM64) - Native Vulkan
- `./gradlew run -Pcmake.preset=windows-egl` - OpenGL ES 3.0 via ANGLE
