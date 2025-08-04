#ifndef __APPLE__

#include "awt_gl_backend.hpp"
#include <mbgl/gl/renderable_resource.hpp>
#include <mbgl/util/logging.hpp>

namespace maplibre_jni
{

    class GLRenderableResource final : public mbgl::gl::RenderableResource
    {
    public:
        explicit GLRenderableResource(GLBackend &backend_) : backend(backend_) {}

        void bind() override {}
        void swap() override {}

    private:
        GLBackend &backend;
    };

    GLBackend::GLBackend(JNIEnv *env, jobject canvas, int width, int height)
        : mbgl::gl::RendererBackend(mbgl::gfx::ContextMode::Unique),
          mbgl::gfx::Renderable(
              mbgl::Size{static_cast<uint32_t>(width), static_cast<uint32_t>(height)},
              std::make_unique<GLRenderableResource>(*this)),
          size({static_cast<uint32_t>(width), static_cast<uint32_t>(height)})
    {

        mbgl::Log::Warning(mbgl::Event::OpenGL,
                           "OpenGL backend stub for Linux/Windows - not yet implemented");
    }

    mbgl::gfx::Renderable &GLBackend::getDefaultRenderable()
    {
        return *this;
    }

    void GLBackend::activate()
    {
    }

    void GLBackend::deactivate()
    {
    }

    void GLBackend::updateAssumedState()
    {
        // Stub - no GL context to reset
    }

    mbgl::gl::ProcAddress GLBackend::getExtensionFunctionPointer(const char *)
    {
        return nullptr;
    }

    void GLBackend::setSize(mbgl::Size newSize)
    {
        size = newSize;
    }

} // namespace maplibre_jni

#endif // !__APPLE__
