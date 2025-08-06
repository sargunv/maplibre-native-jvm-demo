#pragma once

#ifdef _WIN32

#include "gl_context_strategy.hpp"
#include <windows.h>

namespace maplibre_jni
{

    class WGLContextStrategy : public GLContextStrategy
    {
    public:
        WGLContextStrategy() = default;
        ~WGLContextStrategy() override;

        void create(JNIEnv *env, jobject canvas) override;
        void destroy() override;

        void makeCurrent() override;
        void releaseCurrent() override;
        void swapBuffers() override;

        void *getProcAddress(const char *name) override;

    private:
        HWND hwnd = nullptr;
        HDC hdc = nullptr;
        HGLRC hglrc = nullptr;
    };

} // namespace maplibre_jni

#endif // _WIN32