#include "generated/kotlin/main/com_maplibre_jni_JOGLRendererFrontend.h"
#include "jni_jogl_renderer_frontend.hpp"
#include "jni_jogl_backend.hpp"
#include "jni_helpers.hpp"

extern "C" {

JNIEXPORT jlong JNICALL Java_com_maplibre_jni_JOGLRendererFrontend_nativeNew
  (JNIEnv* env, jclass, jlong backendPtr, jfloat pixelRatio) {
    try {
        auto* backend = fromJavaPointer<maplibre_jni::JOGLRendererBackend>(backendPtr);
        
        // Create our JOGL renderer frontend with the backend
        auto frontend = maplibre_jni::JOGLRendererFrontend::create(*backend, pixelRatio);
        
        return toJavaPointer(frontend.release());
    } catch (const std::exception& e) {
        throwJavaException(env, "java/lang/RuntimeException", e.what());
        return 0;
    }
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_JOGLRendererFrontend_nativeDestroy
  (JNIEnv* env, jclass, jlong ptr) {
    if (ptr == 0) {
        return;
    }
    
    auto* frontend = fromJavaPointer<maplibre_jni::JOGLRendererFrontend>(ptr);
    delete frontend;
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_JOGLRendererFrontend_nativeRender
  (JNIEnv* env, jclass, jlong ptr) {
    try {
        auto* frontend = fromJavaPointer<maplibre_jni::JOGLRendererFrontend>(ptr);
        frontend->render();
    } catch (const std::exception& e) {
        throwJavaException(env, "java/lang/RuntimeException", e.what());
    }
}

}