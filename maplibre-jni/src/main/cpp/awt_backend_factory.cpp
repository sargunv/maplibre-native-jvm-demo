#include "awt_backend_factory.hpp"
#include <mbgl/util/logging.hpp>

namespace maplibre_jni
{
    // Factory function to create platform-specific backend
    std::unique_ptr<PlatformBackend> createPlatformBackend(
        JNIEnv *env,
        jobject canvas,
        int width,
        int height,
        const mbgl::gfx::ContextMode contextMode)
    {
        return std::make_unique<PlatformBackend>(env, canvas, width, height);
    }

} // namespace maplibre_jni
