#pragma once

#ifdef USE_WGL_BACKEND

#include <mbgl/gl/renderer_backend.hpp>
#include <mbgl/gfx/renderable.hpp>
#include <mbgl/util/size.hpp>
#include <jni.h>
#include <windows.h>

namespace maplibre_jni
{

    class WGLBackend : public mbgl::gl::RendererBackend,
                       public mbgl::gfx::Renderable
    {
    public:
        WGLBackend(JNIEnv *env, jobject canvas, int width, int height);
        ~WGLBackend() override;

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
        void setFramebufferBinding(uint32_t fbo) { assumeFramebufferBinding(fbo); }
        void setViewport(int32_t x, int32_t y, const mbgl::Size &size) { assumeViewport(x, y, size); }

    private:
        void setupWGLContext(JNIEnv *env, jobject canvas);
        void destroyWGLContext();
        JNIEnv *getEnv();

        JavaVM *javaVM = nullptr;
        jobject canvasRef = nullptr;
        mbgl::Size size;

        // Windows-specific WGL handles
        HWND hwnd = nullptr;
        HDC hdc = nullptr;
        HGLRC hglrc = nullptr;
    };

} // namespace maplibre_jni

#endif // USE_WGL_BACKEND
