#pragma once

#ifndef __APPLE__

#include <mbgl/gl/renderer_backend.hpp>
#include <mbgl/gfx/renderable.hpp>
#include <mbgl/util/size.hpp>
#include <jni.h>

namespace maplibre_jni
{

    class GLBackend final : public mbgl::gl::RendererBackend,
                            public mbgl::gfx::Renderable
    {
    public:
        GLBackend(JNIEnv *env, jobject canvas, int width, int height);
        ~GLBackend() override = default;

        // mbgl::gfx::RendererBackend implementation
        mbgl::gfx::Renderable &getDefaultRenderable() override;

        // mbgl::gl::RendererBackend implementation
        void activate() override;
        void deactivate() override;
        void updateAssumedState() override;
        mbgl::gl::ProcAddress getExtensionFunctionPointer(const char *) override;

        // Size management (needed by AwtCanvasRenderer)
        void setSize(mbgl::Size newSize);

    private:
        mbgl::Size size;
    };

} // namespace maplibre_jni

#endif // !__APPLE__
