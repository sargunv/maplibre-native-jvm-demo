#pragma once

#include <mbgl/gl/renderer_backend.hpp>
#include <mbgl/gfx/renderable.hpp>
#include <mbgl/util/size.hpp>
#include <jni.h>
#include <memory>

namespace maplibre_jni
{
    class GLContextStrategy;

    class GLBackend : public mbgl::gl::RendererBackend,
                      public mbgl::gfx::Renderable
    {
    public:
        GLBackend(JNIEnv *env, jobject canvas, int width, int height,
                  std::unique_ptr<GLContextStrategy> strategy);
        ~GLBackend() override;

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
        mbgl::gfx::Renderable::SwapBehaviour getSwapBehavior() const { return swapBehaviour; }
        void setSwapBehavior(mbgl::gfx::Renderable::SwapBehaviour behaviour) { swapBehaviour = behaviour; }

    private:
        JNIEnv *getEnv();

        JavaVM *javaVM = nullptr;
        jobject canvasRef = nullptr;
        mbgl::Size size;
        std::unique_ptr<GLContextStrategy> contextStrategy;
        mbgl::gfx::Renderable::SwapBehaviour swapBehaviour = mbgl::gfx::Renderable::SwapBehaviour::NoFlush;
    };

} // namespace maplibre_jni