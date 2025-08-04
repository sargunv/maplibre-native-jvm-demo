#include <mbgl/gfx/renderer_backend.hpp>
#include <mbgl/util/logging.hpp>
#include <memory>
#include <jni.h>

namespace maplibre_jni {

// Forward declarations for platform-specific implementations
#ifdef __APPLE__
std::unique_ptr<mbgl::gfx::RendererBackend> createMetalBackend(
    JNIEnv* env, 
    jobject canvas, 
    int width, 
    int height,
    const mbgl::gfx::ContextMode contextMode
);
#else
std::unique_ptr<mbgl::gfx::RendererBackend> createGLBackend(
    JNIEnv* env, 
    jobject canvas, 
    int width, 
    int height,
    const mbgl::gfx::ContextMode contextMode
);
#endif

// Factory function to create platform-specific backend
std::unique_ptr<mbgl::gfx::RendererBackend> createPlatformBackend(
    JNIEnv* env, 
    jobject canvas, 
    int width, 
    int height,
    const mbgl::gfx::ContextMode contextMode
) {
#ifdef __APPLE__
    mbgl::Log::Info(mbgl::Event::General, "Creating Metal backend for macOS");
    return createMetalBackend(env, canvas, width, height, contextMode);
#else
    mbgl::Log::Info(mbgl::Event::General, "Creating OpenGL ES backend");
    return createGLBackend(env, canvas, width, height, contextMode);
#endif
}

} // namespace maplibre_jni