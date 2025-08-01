#include "jni_jogl_backend.hpp"
#include <mbgl/gl/renderable_resource.hpp>
#include <mbgl/util/instrumentation.hpp>
#include <stdexcept>

namespace maplibre_jni {

// Custom renderable resource for JOGL backend
class JOGLRenderableResource final : public mbgl::gl::RenderableResource {
public:
    explicit JOGLRenderableResource(JOGLRendererBackend& backend_)
        : backend(backend_) {}

    void bind() override {
        MLN_TRACE_FUNC();
        
        // Bind default framebuffer
        backend.setFramebufferBinding(0);
        
        // Get the current size and set viewport
        auto size = backend.getSize();
        backend.setViewport(0, 0, size);
    }

    void swap() override {
        MLN_TRACE_FUNC();
        
        backend.swap();
    }

private:
    JOGLRendererBackend& backend;
};

JOGLRendererBackend::JOGLRendererBackend(JNIEnv* env, jobject joglContext, int width, int height)
    : mbgl::gl::RendererBackend(mbgl::gfx::ContextMode::Unique),
      mbgl::gfx::Renderable(mbgl::Size{ static_cast<uint32_t>(width), static_cast<uint32_t>(height) },
                           std::make_unique<JOGLRenderableResource>(*this)) {
    
    // Get JavaVM
    if (env->GetJavaVM(&jvm) != JNI_OK) {
        throw std::runtime_error("Failed to get JavaVM");
    }
    
    // Create global reference to GL context
    glContext = env->NewGlobalRef(joglContext);
    if (!glContext) {
        throw std::runtime_error("Failed to create global reference to GL context");
    }
    
    // Get GL context class and cache method IDs
    jclass contextClass = env->GetObjectClass(joglContext);
    
    // These methods will be implemented in our Kotlin GLContext wrapper
    makeCurrentMethod = env->GetMethodID(contextClass, "makeCurrent", "()V");
    releaseMethod = env->GetMethodID(contextClass, "release", "()V");
    getProcAddressMethod = env->GetMethodID(contextClass, "getProcAddress", "(Ljava/lang/String;)J");
    
    if (!makeCurrentMethod || !releaseMethod || !getProcAddressMethod) {
        env->DeleteGlobalRef(glContext);
        throw std::runtime_error("Failed to find required GL context methods");
    }
    
    env->DeleteLocalRef(contextClass);
}

JOGLRendererBackend::~JOGLRendererBackend() {
    JNIEnv* env = getEnv();
    if (env && glContext) {
        env->DeleteGlobalRef(glContext);
    }
}

JNIEnv* JOGLRendererBackend::getEnv() {
    JNIEnv* env = nullptr;
    jint result = jvm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6);
    
    if (result == JNI_EDETACHED) {
        // Attach current thread
        if (jvm->AttachCurrentThread(reinterpret_cast<void**>(&env), nullptr) != JNI_OK) {
            return nullptr;
        }
    }
    
    return env;
}

void JOGLRendererBackend::activate() {
    MLN_TRACE_FUNC();
    
    JNIEnv* env = getEnv();
    if (!env) {
        throw std::runtime_error("Failed to get JNI environment");
    }
    
    // Make the GL context current
    env->CallVoidMethod(glContext, makeCurrentMethod);
    
    // Check for Java exceptions
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        throw std::runtime_error("Failed to make GL context current");
    }
}

void JOGLRendererBackend::deactivate() {
    MLN_TRACE_FUNC();
    
    JNIEnv* env = getEnv();
    if (!env) {
        return;
    }
    
    // Release the GL context
    env->CallVoidMethod(glContext, releaseMethod);
    
    // Clear any exceptions but don't throw
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        env->ExceptionClear();
    }
}

void JOGLRendererBackend::updateAssumedState() {
    MLN_TRACE_FUNC();
    
    // Update assumed OpenGL state (matching GLFW implementation)
    assumeFramebufferBinding(0);
    setViewport(0, 0, size);
}

mbgl::gl::ProcAddress JOGLRendererBackend::getExtensionFunctionPointer(const char* name) {
    MLN_TRACE_FUNC();
    
    JNIEnv* env = getEnv();
    if (!env) {
        return nullptr;
    }
    
    // Convert C string to Java string
    jstring jName = env->NewStringUTF(name);
    if (!jName) {
        return nullptr;
    }
    
    // Call getProcAddress method
    jlong address = env->CallLongMethod(glContext, getProcAddressMethod, jName);
    
    // Clean up
    env->DeleteLocalRef(jName);
    
    // Check for exceptions
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        return nullptr;
    }
    
    return reinterpret_cast<mbgl::gl::ProcAddress>(address);
}

void JOGLRendererBackend::updateSize(int width, int height) {
    // Update the protected size member directly
    size = mbgl::Size{ static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
    
    // Update assumed state for the new size
    assumeViewport(0, 0, size);
}

void JOGLRendererBackend::swap() {
    // In JOGL, buffer swapping is typically handled by the GLAutoDrawable
    // We don't need to do anything here as JOGL will handle it after our render
}

}