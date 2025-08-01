# maplibre-native-jvm-demo

This project demos the integration of MapLibre Native with a Kotlin JVM application using JNI. It provides a basic setup to render maps using OpenGL ES 3.2 and JOGL.

Be aware that the code is rough and not production-ready. It is intended for educational purposes to demonstrate how to set up MapLibre Native in a JAWT window.

### What works now
- Complete rendering pipeline: Map → Frontend → Backend → JOGL → Display
- Network resource loading: Remote styles and tiles load successfully
- Async event processing: RunLoop processes callbacks in render loop
- MapLibre initialization: All components initialize successfully
- OpenGL ES compatibility: Proper OpenGL ES 3.2 context with framebuffer config
- Map observer callbacks: All events fire correctly (style loaded, map loaded, etc.)

### What isn't yet implemented
- Repaint as needed: The map repaints constantly, but should only repaint when necessary
- User interaction: No mouse/keyboard controls for pan/zoom/rotate
- Memory management: Cleanup in `dispose()` is disabled to avoid crashes
- Runtime styling: Cannot modify or change styles at runtime
- Offline maps: No support for offline tiles or caching yet
- Error handling: No robust error handling or logging implemented
