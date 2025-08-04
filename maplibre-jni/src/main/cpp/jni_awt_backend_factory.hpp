#pragma once

#include <mbgl/gfx/renderer_backend.hpp>
#include <mbgl/util/size.hpp>
#include <memory>
#include <jni.h>

#ifdef __APPLE__
#include "jni_awt_metal_backend.hpp"
#else
#include "jni_awt_gl_backend.hpp"
#endif

namespace maplibre_jni {

// Platform-specific type alias
#ifdef __APPLE__
    using PlatformBackend = MetalBackend;
#else
    using PlatformBackend = GLBackend;
#endif

// Factory function to create platform-specific backend
std::unique_ptr<PlatformBackend> createPlatformBackend(
    JNIEnv* env, 
    jobject canvas, 
    int width, 
    int height,
    const mbgl::gfx::ContextMode contextMode
);

} // namespace maplibre_jni