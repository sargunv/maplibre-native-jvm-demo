#ifndef __APPLE__

#include "awt_vulkan_backend.hpp"
#include <mbgl/vulkan/renderable_resource.hpp>
#include <mbgl/vulkan/context.hpp>
#include <mbgl/util/logging.hpp>

#include <jawt.h>
#include <jawt_md.h>
#include <X11/Xlib.h>

#ifdef __linux__
#define VK_USE_PLATFORM_XLIB_KHR
#include <vulkan/vulkan_xlib.h>
#endif

// VulkanRenderableResource must be in global namespace to match GLFW pattern
class VulkanRenderableResource final : public mbgl::vulkan::SurfaceRenderableResource
{
public:
    explicit VulkanRenderableResource(maplibre_jni::VulkanBackend &backend_)
        : mbgl::vulkan::SurfaceRenderableResource(backend_),
          vulkanBackend(backend_) {}

    void createPlatformSurface() override
    {
#ifdef __linux__
        auto &backendImpl = static_cast<maplibre_jni::VulkanBackend &>(backend);

        if (!backendImpl.getX11Display() || !backendImpl.getX11Window())
        {
            throw std::runtime_error("X11 display or window not available");
        }

        VkXlibSurfaceCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
        createInfo.dpy = static_cast<Display *>(backendImpl.getX11Display());
        createInfo.window = static_cast<Window>(backendImpl.getX11Window());

        VkSurfaceKHR surface_;
        VkResult result = vkCreateXlibSurfaceKHR(
            backendImpl.getInstance().get(),
            &createInfo,
            nullptr,
            &surface_);

        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create X11 surface");
        }

        surface = vk::UniqueSurfaceKHR(
            surface_,
            vk::ObjectDestroy<vk::Instance, vk::DispatchLoaderDynamic>(
                backendImpl.getInstance().get(), nullptr, backendImpl.getDispatcher()));

        mbgl::Log::Info(mbgl::Event::General, "Vulkan X11 surface created successfully");
#else
        throw std::runtime_error("Platform surface creation not implemented for this platform");
#endif
    }

    void bind() override
    {
        // Stub - binding is handled by Vulkan command buffers
    }

    std::vector<const char *> getDeviceExtensions() override
    {
        return {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    }

private:
    maplibre_jni::VulkanBackend &vulkanBackend;
};

namespace maplibre_jni
{

    VulkanBackend::VulkanBackend(JNIEnv *env, jobject canvas, int width, int height)
        : mbgl::vulkan::RendererBackend(mbgl::gfx::ContextMode::Unique),
          mbgl::vulkan::Renderable(
              mbgl::Size{static_cast<uint32_t>(width), static_cast<uint32_t>(height)},
              std::make_unique<VulkanRenderableResource>(*this)),
          size({static_cast<uint32_t>(width), static_cast<uint32_t>(height)})
    {
        env->GetJavaVM(&javaVM);
        canvasRef = env->NewGlobalRef(canvas);
        setupVulkanSurface(env, canvas);
        init();
    }

    VulkanBackend::~VulkanBackend()
    {
        if (canvasRef)
        {
            JNIEnv *env = getEnv();
            if (env)
            {
                env->DeleteGlobalRef(canvasRef);
            }
            canvasRef = nullptr;
        }
    }

    mbgl::gfx::Renderable &VulkanBackend::getDefaultRenderable()
    {
        return *this;
    }

    void VulkanBackend::setSize(mbgl::Size newSize)
    {
        // Update both our local size and the Renderable's size
        size = newSize;
        this->mbgl::vulkan::Renderable::size = newSize;
        
        if (context)
        {
            auto &contextImpl = static_cast<mbgl::vulkan::Context &>(*context);
            contextImpl.requestSurfaceUpdate();
        }
    }

    JNIEnv *VulkanBackend::getEnv()
    {
        JNIEnv *env = nullptr;
        if (javaVM)
        {
            javaVM->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6);
        }
        return env;
    }

    std::vector<const char *> VulkanBackend::getInstanceExtensions()
    {
        auto extensions = mbgl::vulkan::RendererBackend::getInstanceExtensions();

        // Add surface extensions
        extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);

#ifdef __linux__
        // Add X11 surface extension
        extensions.push_back(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
#elif _WIN32
        // Add Windows surface extension
        extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif

        return extensions;
    }

    void VulkanBackend::setupVulkanSurface(JNIEnv *env, jobject canvas)
    {
        // Get JAWT
        JAWT awt;
        awt.version = JAWT_VERSION_9;

        jboolean result = JAWT_GetAWT(env, &awt);
        if (result == JNI_FALSE)
        {
            mbgl::Log::Error(mbgl::Event::General, "JAWT_GetAWT failed");
            return;
        }

        // Get the drawing surface
        JAWT_DrawingSurface *ds = awt.GetDrawingSurface(env, canvas);
        if (!ds)
        {
            mbgl::Log::Error(mbgl::Event::General, "GetDrawingSurface returned null");
            return;
        }

        // Lock the drawing surface
        jint lock = ds->Lock(ds);
        if ((lock & JAWT_LOCK_ERROR) != 0)
        {
            mbgl::Log::Error(mbgl::Event::General, "Error locking drawing surface");
            awt.FreeDrawingSurface(ds);
            return;
        }

        // Get the drawing surface info
        JAWT_DrawingSurfaceInfo *dsi = ds->GetDrawingSurfaceInfo(ds);
        if (!dsi)
        {
            mbgl::Log::Error(mbgl::Event::General, "GetDrawingSurfaceInfo returned null");
            ds->Unlock(ds);
            awt.FreeDrawingSurface(ds);
            return;
        }

#ifdef __linux__
        // Get the X11 drawing surface info
        JAWT_X11DrawingSurfaceInfo *x11Info = (JAWT_X11DrawingSurfaceInfo *)dsi->platformInfo;
        if (!x11Info)
        {
            mbgl::Log::Error(mbgl::Event::General, "Platform info is null");
            ds->FreeDrawingSurfaceInfo(dsi);
            ds->Unlock(ds);
            awt.FreeDrawingSurface(ds);
            return;
        }

        // Store X11 window handles
        x11Display = x11Info->display;
        x11Window = x11Info->drawable;

        mbgl::Log::Info(mbgl::Event::General,
                        "JAWT X11 surface extracted successfully");
#endif

        // We must unlock the surface immediately after getting the handles
        // Otherwise AWT event processing will be blocked
        ds->FreeDrawingSurfaceInfo(dsi);
        ds->Unlock(ds);
        awt.FreeDrawingSurface(ds);
    }

} // namespace maplibre_jni

#endif // !__APPLE__
