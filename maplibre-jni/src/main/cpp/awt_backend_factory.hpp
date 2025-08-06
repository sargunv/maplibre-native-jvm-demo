#pragma once

#include <mbgl/gfx/renderer_backend.hpp>
#include <mbgl/util/size.hpp>
#include <memory>
#include <jni.h>

#ifdef USE_METAL_BACKEND
#include "awt_metal_backend.hpp"
#elif USE_VULKAN_BACKEND
#include "awt_vulkan_backend.hpp"
#elif USE_EGL_BACKEND || USE_WGL_BACKEND || USE_GLX_BACKEND
#include "awt_gl_backend.hpp"
#endif

namespace maplibre_jni
{
#ifdef USE_METAL_BACKEND
    using PlatformBackend = MetalBackend;
#elif USE_VULKAN_BACKEND
    using PlatformBackend = VulkanBackend;
#elif USE_EGL_BACKEND || USE_WGL_BACKEND || USE_GLX_BACKEND
    using PlatformBackend = GLBackend;
#endif

    std::unique_ptr<PlatformBackend> createPlatformBackend(
        JNIEnv *env,
        jobject canvas,
        int width,
        int height,
        const mbgl::gfx::ContextMode contextMode);

} // namespace maplibre_jni
