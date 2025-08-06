#pragma once

#ifdef USE_EGL_BACKEND

#include <mbgl/gl/renderer_backend.hpp>
#include <mbgl/gfx/renderable.hpp>
#include <mbgl/util/size.hpp>
#include <jni.h>

#include <EGL/egl.h>
#include <EGL/eglext.h>

#ifdef _WIN32
#include <windows.h>
#endif

namespace maplibre_jni
{

    class EGLBackend : public mbgl::gl::RendererBackend,
                       public mbgl::gfx::Renderable
    {
    public:
        EGLBackend(JNIEnv *env, jobject canvas, int width, int height);
        ~EGLBackend() override;

        mbgl::gfx::Renderable &getDefaultRenderable() override;
        void setSize(mbgl::Size size);
        mbgl::Size getSize() const { return size; }

    protected:
        void activate() override;
        void deactivate() override;
        mbgl::gl::ProcAddress getExtensionFunctionPointer(const char *name) override;
        void updateAssumedState() override;

    public:
        void swapBuffers();

    private:
        void setupEGLContext(JNIEnv *env, jobject canvas);
        void destroyEGLContext();
        JNIEnv *getEnv();

        JavaVM *javaVM = nullptr;
        jobject canvasRef = nullptr;
        mbgl::Size size;

        // Native window handles
        void *nativeDisplay = nullptr;
        void *nativeWindow = nullptr;

#ifdef _WIN32
        HWND hwnd = nullptr;
#endif

        // EGL handles
        EGLDisplay eglDisplay = EGL_NO_DISPLAY;
        EGLContext eglContext = EGL_NO_CONTEXT;
        EGLSurface eglSurface = EGL_NO_SURFACE;
        EGLConfig eglConfig = nullptr;
    };

} // namespace maplibre_jni

#endif // USE_EGL_BACKEND
