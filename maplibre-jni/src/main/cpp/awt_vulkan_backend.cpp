#ifndef __APPLE__

#include "awt_vulkan_backend.hpp"
#include <mbgl/vulkan/renderable_resource.hpp>
#include <mbgl/util/logging.hpp>

namespace maplibre_jni
{

    class VulkanRenderableResource final : public mbgl::vulkan::SurfaceRenderableResource
    {
    public:
        explicit VulkanRenderableResource(VulkanBackend &backend_)
            : mbgl::vulkan::SurfaceRenderableResource(backend_) {}

        void createPlatformSurface() override
        {
            // Stub - will be implemented when we add X11/Win32 surface creation
            mbgl::Log::Warning(mbgl::Event::General,
                             "Vulkan surface creation not yet implemented");
        }
        
        void bind() override
        {
            // Stub - binding is handled by Vulkan command buffers
        }
    };

    VulkanBackend::VulkanBackend(JNIEnv *env, jobject canvas, int width, int height)
        : mbgl::vulkan::RendererBackend(mbgl::gfx::ContextMode::Unique),
          mbgl::gfx::Renderable(
              mbgl::Size{static_cast<uint32_t>(width), static_cast<uint32_t>(height)},
              std::make_unique<VulkanRenderableResource>(*this)),
          size({static_cast<uint32_t>(width), static_cast<uint32_t>(height)})
    {

        mbgl::Log::Warning(mbgl::Event::General,
                           "Vulkan backend stub for Linux/Windows - not yet implemented");
    }

    mbgl::gfx::Renderable &VulkanBackend::getDefaultRenderable()
    {
        return *this;
    }

    void VulkanBackend::activate()
    {
        // Stub - Vulkan doesn't need activation like GL
    }

    void VulkanBackend::deactivate()
    {
        // Stub - Vulkan doesn't need deactivation like GL
    }

    void VulkanBackend::setSize(mbgl::Size newSize)
    {
        size = newSize;
    }

} // namespace maplibre_jni

#endif // !__APPLE__
