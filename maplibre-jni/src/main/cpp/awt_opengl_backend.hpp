#pragma once

#ifndef __APPLE__

#include <mbgl/gfx/renderable.hpp>
#include <mbgl/gl/renderer_backend.hpp>
#include <mbgl/util/size.hpp>
#include <jni.h>

#include <EGL/egl.h>
#include <GL/gl.h>

namespace maplibre_jni
{

    class OpenGLBackend final : public mbgl::gl::RendererBackend,
                                public mbgl::gfx::Renderable
    {
    public:
        OpenGLBackend(JNIEnv *env, jobject canvas, int width, int height);
        ~OpenGLBackend() override;

        // mbgl::gfx::RendererBackend implementation
        mbgl::gfx::Renderable &getDefaultRenderable() override;

        // Size management
        void setSize(mbgl::Size size);
        mbgl::Size getSize() const { return size; }
        
        // Public methods for RenderableResource
        void swapBuffers();

    protected:
        // mbgl::gl::RendererBackend overrides
        void activate() override;
        void deactivate() override;
        mbgl::gl::ProcAddress getExtensionFunctionPointer(const char *name) override;
        void updateAssumedState() override;

    private:
        // EGL/OpenGL setup
        void setupOpenGLContext(JNIEnv *env, jobject canvas);
        void destroyOpenGLContext();
        JNIEnv *getEnv();

        mbgl::Size size;

        // EGL objects
        EGLDisplay eglDisplay = EGL_NO_DISPLAY;
        EGLContext eglContext = EGL_NO_CONTEXT;
        EGLSurface eglSurface = EGL_NO_SURFACE;
        EGLConfig eglConfig = nullptr;

        // Native display and window handles (stored as void* to avoid X11 header pollution)
        void *nativeDisplay = nullptr;
        void *nativeWindow = nullptr;

        // JNI references
        JavaVM *javaVM = nullptr;
        jobject canvasRef = nullptr;
    };

} // namespace maplibre_jni

#endif // !__APPLE__