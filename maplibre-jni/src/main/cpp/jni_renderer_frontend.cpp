#include "generated/kotlin/main/com_maplibre_jni_RendererFrontend.h"
#include "jni_renderer_frontend.hpp"
#include "jni_helpers.hpp"

extern "C" {

JNIEXPORT jlong JNICALL Java_com_maplibre_jni_RendererFrontend_nativeNew
  (JNIEnv* env, jclass, jlong backendPtr, jfloat pixelRatio) {
    try {
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

}