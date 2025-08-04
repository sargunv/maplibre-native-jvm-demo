#pragma once

#ifndef __APPLE__

#include <mbgl/vulkan/renderable_resource.hpp>
#include <mbgl/vulkan/renderer_backend.hpp>
#include <mbgl/gfx/renderer_backend.hpp>
#include <mbgl/util/size.hpp>
#include <jni.h>

namespace maplibre_jni {

class VulkanBackend final : public mbgl::vulkan::RendererBackend,
                           public mbgl::gfx::Renderable {
public:
    VulkanBackend(JNIEnv* env, jobject canvas, int width, int height);
    ~VulkanBackend() override = default;

    // mbgl::gfx::RendererBackend implementation
    mbgl::gfx::Renderable& getDefaultRenderable() override;
    
    // Size management
    void setSize(mbgl::Size size);

protected:
    // mbgl::vulkan::RendererBackend overrides
    void activate() override;
    void deactivate() override;

private:
    mbgl::Size size;
};

} // namespace maplibre_jni

#endif // !__APPLE__