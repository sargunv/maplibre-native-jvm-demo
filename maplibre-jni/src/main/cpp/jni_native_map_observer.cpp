#include "com_maplibre_jni_NativeMapObserver.h"
#include "jni_map_observer.hpp"
#include "jni_helpers.hpp"

extern "C" {

JNIEXPORT jlong JNICALL Java_com_maplibre_jni_NativeMapObserver_nativeNew
  (JNIEnv* env, jclass, jobject observer) {
    try {
        auto* nativeObserver = new maplibre_jni::JniMapObserver(env, observer);
        return toJavaPointer(nativeObserver);
    } catch (const std::exception& e) {
        throwJavaException(env, "java/lang/RuntimeException", e.what());
        return 0;
    }
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_NativeMapObserver_nativeDestroy
  (JNIEnv* env, jclass, jlong ptr) {
    delete fromJavaPointer<maplibre_jni::JniMapObserver>(ptr);
}

}