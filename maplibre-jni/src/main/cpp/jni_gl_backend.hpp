#pragma once

#include "jni_jawt_backend.hpp"
#include <mbgl/gl/renderer_backend.hpp>
#include <mbgl/gfx/renderable.hpp>
#include <memory>

#ifndef __APPLE__

// Platform-agnostic EGL includes
#include <EGL/egl.h>
#include <EGL/eglext.h>

namespace maplibre_jni {

// OpenGL ES renderer backend for Linux/Windows using EGL
class GLBackend : public mbgl::gl::RendererBackend, 
                  public mbgl::gfx::Renderable,
                  public JAWTRendererBackend {
public:
    GLBackend(JNIEnv* env, jobject canvas, int width, int height);
    ~GLBackend() override;

    // JAWTRendererBackend implementation
    void updateSize(int width, int height) override;
    void swap() override;
    void* getRendererBackend() override { return static_cast<mbgl::gl::RendererBackend*>(this); }
    
    // gfx::RendererBackend implementation
    mbgl::gfx::Renderable& getDefaultRenderable() override { return *this; }
    
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
    
    // Helper methods
    void initializeEGL();
    void* getNativeWindowHandle(JNIEnv* env, jobject canvas);
    void releaseNativeWindowHandle();
    
    // Platform-specific EGL setup
    EGLDisplay getPlatformDisplay();
    
    // Store JAWT structures for cleanup
    void* jawtDrawingSurface = nullptr;
    void* jawtDrawingSurfaceInfo = nullptr;
};

} // namespace maplibre_jni

#endif // !__APPLE__