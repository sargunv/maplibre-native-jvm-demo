# kotlin-maplibre-native

MapLibre Native JVM bindings for desktop platforms. This project enables MapLibre Native rendering in Java/Kotlin desktop applications using AWT/Swing with native graphics backends.

### Platform support

| Renderer        | GL ES     | Desktop GL | Vulkan       | Metal |
| --------------- | --------- | ---------- | ------------ | ----- |
| macOS arm64     | ❌ (ANGLE) | N/A        | ✅ (MoltenVK) | ✅     |
| macOS x64       | ❌ (ANGLE) | N/A        | ❔ (MoltenVK) | ❔     |
| Linux X11 arm64 | ✅         | ❌          | ✅            | N/A   |
| Linux X11 x64   | ✅         | ❌          | ✅            | N/A   |
| Windows arm64   | ⚠️ (ANGLE) | ❌          | ✅            | N/A   |
| Windows x64     | ✅ (ANGLE) | ✅          | ✅            | N/A   |

❔ = Implemented but not tested  
⚠️ = Implemented but doesn't work

### Known issues

- On Windows arm64, the app crashes shortly after startup (the map does render though!)

### What works
- Complete rendering pipeline with MapLibre Native integration
- Network resource loading (remote styles and tiles)
- Map observer callbacks and event handling
- User interaction (pan, zoom, rotate via mouse/keyboard)

### What isn't implemented yet
- Runtime styling
- Offline maps
- A bunch of other misc API methods
