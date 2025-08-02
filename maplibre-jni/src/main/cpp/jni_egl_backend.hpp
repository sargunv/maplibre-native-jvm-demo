#pragma once

#include <mbgl/gl/renderer_backend.hpp>
#include <mbgl/gfx/renderable.hpp>
#include <jni.h>
#include <memory>

// Platform-agnostic EGL includes
#include <EGL/egl.h>
#include <EGL/eglext.h>

namespace maplibre_jni {

// EGL-based OpenGL ES renderer backend that works on all platforms
class EGLRendererBackend : public mbgl::gl::RendererBackend, public mbgl::gfx::Renderable {
public:
    EGLRendererBackend(JNIEnv* env, jobject canvas, int width, int height);
    ~EGLRendererBackend() override;

    // gfx::RendererBackend implementation
    mbgl::gfx::Renderable& getDefaultRenderable() override { return *this; }
    
    // Called when the view size changes
    void updateSize(int width, int height);
    
    // Method to swap buffers (called after rendering)
    void swap();
    
protected:
    // gfx::RendererBackend implementation
    void activate() override;
    void deactivate() override;
    
    // gl::RendererBackend implementation
    void updateAssumedState() override;
    mbgl::gl::ProcAddress getExtensionFunctionPointer(const char* name) override;
    
private:
    // EGL objects
    EGLDisplay display = EGL_NO_DISPLAY;
    EGLContext context = EGL_NO_CONTEXT;
    EGLSurface surface = EGL_NO_SURFACE;
    EGLConfig config = nullptr;
    
    // Native window handle
    void* nativeWindow = nullptr;
    
    // JNI references
    JavaVM* jvm = nullptr;
    jobject canvasRef = nullptr;  // Global reference to the Java canvas
    
    // Helper methods
    void initializeEGL();
    void* getNativeWindowHandle(JNIEnv* env, jobject canvas);
    void releaseNativeWindowHandle();
    JNIEnv* getEnv();
    
    // Platform-specific EGL setup
    EGLDisplay getPlatformDisplay();
    
#ifdef __APPLE__
    // macOS-specific methods for handling CALayer
    void* getNativeWindowHandleMacOS(JNIEnv* env, jobject canvas);
    void releaseNativeWindowHandleMacOS();
    
    // Store JAWT structures for cleanup
    void* jawtDrawingSurface = nullptr;
    void* jawtDrawingSurfaceInfo = nullptr;
#endif
};

} // namespace maplibre_jni