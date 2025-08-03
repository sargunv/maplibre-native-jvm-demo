#include "jni_jawt_backend.hpp"
#include "jni_helpers.hpp"
#include <memory>
#include <unordered_map>

extern "C" {

// Map of native pointers to backend instances
static std::unordered_map<jlong, std::unique_ptr<maplibre_jni::JAWTRendererBackend>> backendMap;

JNIEXPORT jlong JNICALL Java_com_maplibre_jni_JAWTRendererBackend_nativeCreate(
    JNIEnv* env, jclass, jobject canvas, jint width, jint height) {
    try {
        auto backend = maplibre_jni::createPlatformBackend(env, canvas, width, height);
        jlong ptr = toJavaPointer(backend.get());
        backendMap[ptr] = std::move(backend);
        return ptr;
    } catch (const std::exception& e) {
        jclass exClass = env->FindClass("java/lang/RuntimeException");
        env->ThrowNew(exClass, e.what());
        return 0;
    }
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_JAWTRendererBackend_nativeDestroy(
    JNIEnv*, jclass, jlong ptr) {
    backendMap.erase(ptr);
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_JAWTRendererBackend_nativeUpdateSize(
    JNIEnv*, jclass, jlong ptr, jint width, jint height) {
    auto it = backendMap.find(ptr);
    if (it != backendMap.end()) {
        it->second->updateSize(width, height);
    }
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_JAWTRendererBackend_nativeSwap(
    JNIEnv*, jclass, jlong ptr) {
    auto it = backendMap.find(ptr);
    if (it != backendMap.end()) {
        it->second->swap();
    }
}

JNIEXPORT jlong JNICALL Java_com_maplibre_jni_JAWTRendererBackend_nativeGetRendererBackend(
    JNIEnv*, jclass, jlong ptr) {
    auto it = backendMap.find(ptr);
    if (it != backendMap.end()) {
        return toJavaPointer(it->second->getRendererBackend());
    }
    return 0;
}

} // extern "C"