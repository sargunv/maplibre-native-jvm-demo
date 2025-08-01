#include "generated/kotlin/main/com_maplibre_jni_JOGLRendererBackend.h"
#include "jni_jogl_backend.hpp"
#include "jni_helpers.hpp"

extern "C" {

JNIEXPORT jlong JNICALL Java_com_maplibre_jni_JOGLRendererBackend_nativeNew
  (JNIEnv* env, jclass, jobject glContext, jint width, jint height) {
    try {
        auto* backend = new maplibre_jni::JOGLRendererBackend(env, glContext, width, height);
        return toJavaPointer(backend);
    } catch (const std::exception& e) {
        throwJavaException(env, "java/lang/RuntimeException", e.what());
        return 0;
    }
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_JOGLRendererBackend_nativeDestroy
  (JNIEnv* env, jclass, jlong ptr) {
    delete fromJavaPointer<maplibre_jni::JOGLRendererBackend>(ptr);
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_JOGLRendererBackend_nativeUpdateSize
  (JNIEnv* env, jclass, jlong ptr, jint width, jint height) {
    auto* backend = fromJavaPointer<maplibre_jni::JOGLRendererBackend>(ptr);
    backend->updateSize(width, height);
}

}