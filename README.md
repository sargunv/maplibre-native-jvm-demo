# maplibre-native-jvm-demo

This project demos the integration of MapLibre Native with a Kotlin JVM application using JNI. It provides a cross-platform setup to render maps using native graphics APIs (Metal on macOS, GL/Vulkan on Linux/Windows - in development).

Be aware that the code is rough and not production-ready. It is intended for educational purposes to demonstrate how to set up MapLibre Native in a JAWT window.

### What works now
- Complete rendering pipeline: Map → Frontend → Backend → Native API → Display
- Network resource loading: Remote styles and tiles load successfully
- MapLibre initialization: All components initialize successfully
- Native Metal rendering on macOS
- Map observer callbacks: All events fire correctly (style loaded, map loaded, etc.)
- Repaint as needed: The map repaints when dirty
- User interaction: Mouse/keyboard controls for pan/zoom/rotate

### What isn't yet implemented
- Linux/Windows support: Vulkan or GL backend is stubbed but not yet implemented
- Runtime styling: Cannot modify or change styles at runtime
- Offline maps: No support for offline tiles or caching yet
- Error handling: No robust error handling or logging implemented
