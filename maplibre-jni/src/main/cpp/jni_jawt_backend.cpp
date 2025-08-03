#include "jni_jawt_backend.hpp"
#include <stdexcept>

namespace maplibre_jni {

JAWTRendererBackend::JAWTRendererBackend(JNIEnv* env, jobject canvas, int width_, int height_)
    : width(width_), height(height_) {
    
    // Get JavaVM
    if (env->GetJavaVM(&jvm) != JNI_OK) {
        throw std::runtime_error("Failed to get JavaVM");
    }
    
    // Create global reference to canvas
    canvasRef = env->NewGlobalRef(canvas);
    if (!canvasRef) {
        throw std::runtime_error("Failed to create global reference to canvas");
    }
}

JAWTRendererBackend::~JAWTRendererBackend() {
    if (canvasRef) {
        JNIEnv* env = getEnv();
        if (env) {
            env->DeleteGlobalRef(canvasRef);
        }
        canvasRef = nullptr;
    }
}

JNIEnv* JAWTRendererBackend::getEnv() {
    JNIEnv* env = nullptr;
    if (jvm) {
        jint result = jvm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6);
        if (result == JNI_EDETACHED) {
            if (jvm->AttachCurrentThread(reinterpret_cast<void**>(&env), nullptr) != JNI_OK) {
                return nullptr;
            }
        }
    }
    return env;
}

} // namespace maplibre_jni