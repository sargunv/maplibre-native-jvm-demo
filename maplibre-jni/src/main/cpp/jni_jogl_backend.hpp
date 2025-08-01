#pragma once

#include <mbgl/gl/renderer_backend.hpp>
#include <mbgl/gfx/renderable.hpp>
#include <jni.h>
#include <memory>

namespace maplibre_jni {

// JOGL-based OpenGL renderer backend
class JOGLRendererBackend : public mbgl::gl::RendererBackend, public mbgl::gfx::Renderable {
public:
    JOGLRendererBackend(JNIEnv* env, jobject joglContext, int width, int height);
    ~JOGLRendererBackend() override;

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
    JavaVM* jvm;
    jobject glContext;  // Global reference to JOGL GL context object
    
    // Cached method IDs for performance
    jmethodID makeCurrentMethod;
    jmethodID releaseMethod;
    jmethodID getProcAddressMethod;
    
    JNIEnv* getEnv();
};

}