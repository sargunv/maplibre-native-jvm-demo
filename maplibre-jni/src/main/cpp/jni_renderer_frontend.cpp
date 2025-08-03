#include "generated/kotlin/main/com_maplibre_jni_RendererFrontend.h"
#include "jni_renderer_frontend.hpp"
#include "jni_helpers.hpp"
#include <functional>

// Global references for callbacks
static JavaVM* javaVM = nullptr;

extern "C" {

JNIEXPORT jlong JNICALL Java_com_maplibre_jni_RendererFrontend_nativeNew
  (JNIEnv* env, jclass, jlong backendPtr, jfloat pixelRatio) {
    try {
        // Store JavaVM for later use in callbacks
        if (javaVM == nullptr) {
            env->GetJavaVM(&javaVM);
        }
        
        // Accept any backend type as a generic RendererBackend
        auto* backend = reinterpret_cast<mbgl::gfx::RendererBackend*>(backendPtr);
        
        // Create our renderer frontend with the backend
        auto frontend = maplibre_jni::RendererFrontend::create(*backend, pixelRatio);
        
        return toJavaPointer(frontend.release());
    } catch (const std::exception& e) {
        throwJavaException(env, "java/lang/RuntimeException", e.what());
        return 0;
    }
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_RendererFrontend_nativeDestroy
  (JNIEnv* env, jclass, jlong ptr) {
    if (ptr == 0) {
        return;
    }
    
    auto* frontend = fromJavaPointer<maplibre_jni::RendererFrontend>(ptr);
    delete frontend;
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_RendererFrontend_nativeRender
  (JNIEnv* env, jclass, jlong ptr) {
    try {
        auto* frontend = fromJavaPointer<maplibre_jni::RendererFrontend>(ptr);
        frontend->render();
    } catch (const std::exception& e) {
        throwJavaException(env, "java/lang/RuntimeException", e.what());
    }
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_RendererFrontend_nativeSetUpdateCallback
  (JNIEnv* env, jclass, jlong ptr, jobject callback) {
    try {
        auto* frontend = fromJavaPointer<maplibre_jni::RendererFrontend>(ptr);
        
        if (callback == nullptr) {
            // Clear the callback
            frontend->setUpdateCallback(nullptr);
        } else {
            // Create a global reference to the callback
            jobject globalCallback = env->NewGlobalRef(callback);
            
            // Set the C++ callback that will invoke the Java callback
            frontend->setUpdateCallback([globalCallback]() {
                if (javaVM == nullptr) return;
                
                JNIEnv* env = nullptr;
                bool needsDetach = false;
                
                // Get JNI environment
                int getEnvResult = javaVM->GetEnv((void**)&env, JNI_VERSION_1_6);
                if (getEnvResult == JNI_EDETACHED) {
                    javaVM->AttachCurrentThread((void**)&env, nullptr);
                    needsDetach = true;
                } else if (getEnvResult != JNI_OK) {
                    return;
                }
                
                // Call the Java callback
                jclass clazz = env->GetObjectClass(globalCallback);
                jmethodID method = env->GetMethodID(clazz, "run", "()V");
                if (method != nullptr) {
                    env->CallVoidMethod(globalCallback, method);
                }
                env->DeleteLocalRef(clazz);
                
                // Detach if we attached
                if (needsDetach) {
                    javaVM->DetachCurrentThread();
                }
            });
        }
    } catch (const std::exception& e) {
        throwJavaException(env, "java/lang/RuntimeException", e.what());
    }
}

}