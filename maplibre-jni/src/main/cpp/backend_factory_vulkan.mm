#include "jni_helpers.hpp"
#include "surface_descriptors.hpp"
#include <mbgl/gfx/renderer_backend.hpp>
#include <mbgl/vulkan/renderer_backend.hpp>
#include <mbgl/vulkan/renderable_resource.hpp>
#include <mbgl/vulkan/context.hpp>
#include <mbgl/util/size.hpp>

#if defined(__APPLE__)
#define VK_USE_PLATFORM_METAL_EXT
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_metal.h>
#import <QuartzCore/CAMetalLayer.h>
#endif

using namespace mbgl;
using namespace maplibre_jni;

namespace {

class VulkanRenderableResourceExt final : public mbgl::vulkan::SurfaceRenderableResource {
public:
    VulkanRenderableResourceExt(mbgl::vulkan::RendererBackend& backend, void* metalLayer)
        : mbgl::vulkan::SurfaceRenderableResource(backend)
#if defined(__APPLE__)
        , caMetalLayer(reinterpret_cast<CAMetalLayer*>(metalLayer))
#endif
    {}

    void createPlatformSurface() override {
#if defined(__APPLE__)
        // Create a VkSurfaceKHR using VK_EXT_metal_surface from CAMetalLayer
        auto& be = static_cast<mbgl::vulkan::RendererBackend&>(backend);
        const auto& instance = be.getInstance();
        auto dispatcher = be.getDispatcher();

        VkMetalSurfaceCreateInfoEXT createInfo{};
        createInfo.sType = (VkStructureType)VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.pLayer = (const CAMetalLayer*)caMetalLayer;

        VkSurfaceKHR s{};
        auto fpCreateMetalSurface = reinterpret_cast<PFN_vkCreateMetalSurfaceEXT>(
            vkGetInstanceProcAddr(instance.get(), "vkCreateMetalSurfaceEXT")
        );
        if (!fpCreateMetalSurface) {
            throw std::runtime_error("vkCreateMetalSurfaceEXT not available");
        }
        VkResult res = fpCreateMetalSurface(instance.get(), &createInfo, nullptr, &s);
        if (res != VK_SUCCESS) {
            throw std::runtime_error("Failed to create Metal VkSurface");
        }
        surface = vk::UniqueSurfaceKHR(
            s,
            vk::ObjectDestroy<vk::Instance, vk::DispatchLoaderDynamic>(instance.get(), nullptr, dispatcher)
        );
#else
        // TODO: Implement Win32/X11 creation paths in a platform-specific file.
        throw std::runtime_error("Vulkan backend not implemented for this platform");
#endif
    }

    void bind() override {}

    std::vector<const char*> getDeviceExtensions() override { return { VK_KHR_SWAPCHAIN_EXTENSION_NAME }; }

#if defined(__APPLE__)
    CAMetalLayer* caMetalLayer{nullptr};
#endif
};

class VulkanBackend2 : public mbgl::vulkan::RendererBackend, public mbgl::vulkan::Renderable {
public:
    VulkanBackend2(void* metalLayer, mbgl::gfx::ContextMode mode)
        : mbgl::vulkan::RendererBackend(mode)
        , mbgl::vulkan::Renderable(mbgl::Size{0,0}, std::make_unique<VulkanRenderableResourceExt>(*this, metalLayer)) {}

    void setSize(mbgl::Size s) {
        this->mbgl::vulkan::Renderable::size = s;
#if defined(__APPLE__)
        // Update drawable size on layer if available
        auto& res = getResource<VulkanRenderableResourceExt>();
        if (res.caMetalLayer) {
            res.caMetalLayer.drawableSize = CGSizeMake(s.width, s.height);
        }
#endif
        if (context) {
            static_cast<mbgl::vulkan::Context&>(*context).requestSurfaceUpdate();
        }
    }

    mbgl::gfx::Renderable& getDefaultRenderable() override { return *this; }
    void activate() override {}
    void deactivate() override {}

    std::vector<const char*> getInstanceExtensions() override {
        auto exts = mbgl::vulkan::RendererBackend::getInstanceExtensions();
        exts.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
#if defined(__APPLE__)
        exts.push_back(VK_EXT_METAL_SURFACE_EXTENSION_NAME);
        exts.push_back("VK_KHR_portability_enumeration");
#endif
        return exts;
    }
};

} // namespace

extern "C" {

JNIEXPORT jlong JNICALL Java_org_maplibre_kmp_native_internal_Native_createVulkanBackend(JNIEnv* env, jclass, jlong surfaceDescPtr, jint width, jint height, jfloat /*pixelRatio*/, jint /*contextMode*/) {
    try {
        auto* desc = fromJavaPointer<SurfaceDescriptor>(surfaceDescPtr);
        if (!desc || desc->kind != SurfaceKind::Mac) throw std::runtime_error("Invalid macOS surface descriptor");
        auto* layer = (void*)desc->u.mac.caLayer;
        auto* backend = new VulkanBackend2(layer, mbgl::gfx::ContextMode::Unique);
        backend->setSize(mbgl::Size{static_cast<uint32_t>(width), static_cast<uint32_t>(height)});
        backend->init();
        return toJavaPointer(static_cast<gfx::RendererBackend*>(backend));
    } catch (const std::exception& e) {
        throwJavaException(env, "java/lang/RuntimeException", e.what());
        return 0;
    }
}

JNIEXPORT jlong JNICALL Java_org_maplibre_kmp_native_internal_Native_createDefaultBackend(JNIEnv* env, jclass, jlong surfaceDescPtr, jint width, jint height, jfloat /*pixelRatio*/, jint /*contextMode*/) {
    // Inline Vulkan backend creation for default path
    try {
        auto* desc = fromJavaPointer<SurfaceDescriptor>(surfaceDescPtr);
        if (!desc || desc->kind != SurfaceKind::Mac) throw std::runtime_error("Invalid macOS surface descriptor");
        auto* layer = (void*)desc->u.mac.caLayer;
        auto* backend = new VulkanBackend2(layer, mbgl::gfx::ContextMode::Unique);
        backend->setSize(mbgl::Size{static_cast<uint32_t>(width), static_cast<uint32_t>(height)});
        backend->init();
        return toJavaPointer(static_cast<gfx::RendererBackend*>(backend));
    } catch (const std::exception& e) {
        throwJavaException(env, "java/lang/RuntimeException", e.what());
        return 0;
    }
}

JNIEXPORT void JNICALL Java_org_maplibre_kmp_native_internal_Native_backendSetSize(JNIEnv* env, jclass, jlong backendPtr, jint width, jint height) {
    try {
        auto* backend = reinterpret_cast<VulkanBackend2*>(backendPtr);
        backend->setSize(mbgl::Size{static_cast<uint32_t>(width), static_cast<uint32_t>(height)});
    } catch (const std::exception& e) {
        throwJavaException(env, "java/lang/RuntimeException", e.what());
    }
}

JNIEXPORT jlong JNICALL Java_org_maplibre_kmp_native_internal_Native_backendGetThreadPoolPtr(JNIEnv* env, jclass, jlong backendPtr) {
    try {
        auto* backend = reinterpret_cast<gfx::RendererBackend*>(backendPtr);
        return reinterpret_cast<jlong>(&backend->getThreadPool());
    } catch (const std::exception& e) {
        throwJavaException(env, "java/lang/RuntimeException", e.what());
        return 0;
    }
}

}
