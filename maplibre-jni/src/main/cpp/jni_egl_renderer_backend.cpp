#include "generated/kotlin/main/com_maplibre_jni_EGLRendererBackend.h"
#include "jni_helpers.hpp"
#include "jni_egl_backend.hpp"

extern "C" {

JNIEXPORT jlong JNICALL Java_com_maplibre_jni_EGLRendererBackend_nativeCreate
  (JNIEnv* env, jclass, jobject canvas, jint width, jint height) {
    try {
        auto backend = new maplibre_jni::EGLRendererBackend(env, canvas, width, height);
        return toJavaPointer(backend);
    } catch (const std::exception& e) {
        jclass exClass = env->FindClass("java/lang/RuntimeException");
        env->ThrowNew(exClass, e.what());
        return 0;
    }
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_EGLRendererBackend_nativeDestroy
  (JNIEnv*, jclass, jlong ptr) {
    auto backend = fromJavaPointer<maplibre_jni::EGLRendererBackend>(ptr);
    delete backend;
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_EGLRendererBackend_nativeUpdateSize
  (JNIEnv*, jclass, jlong ptr, jint width, jint height) {
    auto backend = fromJavaPointer<maplibre_jni::EGLRendererBackend>(ptr);
    backend->updateSize(width, height);
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_EGLRendererBackend_nativeSwap
  (JNIEnv*, jclass, jlong ptr) {
    auto backend = fromJavaPointer<maplibre_jni::EGLRendererBackend>(ptr);
    backend->swap();
}

}