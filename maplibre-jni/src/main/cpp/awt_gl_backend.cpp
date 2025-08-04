#ifndef __APPLE__

#include "awt_gl_backend.hpp"
#include <mbgl/gl/context.hpp>
#include <mbgl/util/logging.hpp>
#include <memory>
#include <jni.h>
#include <jawt.h>
#include <jawt_md.h>

namespace maplibre_jni {

// OpenGL ES backend implementation
GLBackend::GLBackend(JNIEnv* env, jobject canvas, int width, int height)
    : mbgl::gl::RendererBackend(mbgl::gfx::ContextMode::Unique),
      size({static_cast<uint32_t>(width), static_cast<uint32_t>(height)}) {
    
    // TODO: Implement EGL context creation and JAWT surface binding
    mbgl::Log::Warning(mbgl::Event::OpenGL, 
        "OpenGL ES backend not yet fully implemented for Linux/Windows. "
        "This is a placeholder for future support.");
}

// mbgl::gfx::RendererBackend implementation
mbgl::gfx::Renderable& GLBackend::getDefaultRenderable() { 
    return *this; 
}

// mbgl::gl::RendererBackend implementation  
void GLBackend::activate() {}

void GLBackend::deactivate() {}

void GLBackend::updateAssumedState() {
    // Reset GL state assumptions
    getContext<mbgl::gl::Context>().resetState();
}

// mbgl::gfx::Renderable implementation
void GLBackend::setSize(mbgl::Size newSize) {
    size = newSize;
}

mbgl::Size GLBackend::getSize() const {
    return size;
}

void GLBackend::swap() {
    // TODO: Implement buffer swap via EGL
}

// Factory function
std::unique_ptr<mbgl::gfx::RendererBackend> createGLBackend(
    JNIEnv* env, 
    jobject canvas, 
    int width, 
    int height,
    const mbgl::gfx::ContextMode contextMode
) {
    return std::make_unique<GLBackend>(env, canvas, width, height);
}

} // namespace maplibre_jni

#endif // !__APPLE__