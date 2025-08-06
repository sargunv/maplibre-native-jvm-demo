#pragma once

#include "gl_context_strategy.hpp"
#include <GL/glx.h>
#include <X11/Xlib.h>

namespace maplibre_jni
{

    class GLXContextStrategy : public GLContextStrategy
    {
    public:
        GLXContextStrategy() = default;
        ~GLXContextStrategy() override;

        // Context lifecycle
        void create(JNIEnv *env, jobject canvas) override;
        void destroy() override;

        // Context management
        void makeCurrent() override;
        void releaseCurrent() override;
        void swapBuffers() override;

        // GL function loading
        void *getProcAddress(const char *name) override;

    private:
        Display *display = nullptr;
        Window window = 0;
        GLXContext context = nullptr;
        GLXFBConfig fbConfig = nullptr;
    };

} // namespace maplibre_jni