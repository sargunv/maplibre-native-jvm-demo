#pragma once

#include <jni.h>
#include <memory>

namespace maplibre_jni {

// Base class for JAWT-based renderer backends
// Subclasses will implement platform-specific rendering (Metal, OpenGL ES)
class JAWTRendererBackend {
public:
    JAWTRendererBackend(JNIEnv* env, jobject canvas, int width, int height);
    virtual ~JAWTRendererBackend();
    
    // Update the size of the rendering surface
    virtual void updateSize(int width, int height) = 0;
    
    // Swap buffers after rendering
    virtual void swap() = 0;
    
    // Get the underlying MapLibre renderer backend
    virtual void* getRendererBackend() = 0;
    
protected:
    // JNI environment helper
    JNIEnv* getEnv();
    
    // Canvas reference
    jobject canvasRef = nullptr;  // Global reference to the Java canvas
    JavaVM* jvm = nullptr;
    
    // Size
    int width;
    int height;
};

// Factory function to create platform-specific backend
std::unique_ptr<JAWTRendererBackend> createPlatformBackend(JNIEnv* env, jobject canvas, int width, int height);

} // namespace maplibre_jni