#include "generated/kotlin/main/com_maplibre_jni_Size.h"
#include "mbgl/util/size.hpp"
#include "jni_helpers.hpp"

extern "C" {

JNIEXPORT jlong JNICALL Java_com_maplibre_jni_Size_nativeNew
  (JNIEnv* env, jclass, jint width, jint height) {
    auto* size = new mbgl::Size(width, height);
    return toJavaPointer(size);
}

JNIEXPORT jint JNICALL Java_com_maplibre_jni_Size_nativeWidth
  (JNIEnv* env, jclass, jlong ptr) {
    auto* size = fromJavaPointer<mbgl::Size>(ptr);
    return static_cast<jint>(size->width);
}

JNIEXPORT jint JNICALL Java_com_maplibre_jni_Size_nativeHeight
  (JNIEnv* env, jclass, jlong ptr) {
    auto* size = fromJavaPointer<mbgl::Size>(ptr);
    return static_cast<jint>(size->height);
}

JNIEXPORT jint JNICALL Java_com_maplibre_jni_Size_nativeArea
  (JNIEnv* env, jclass, jlong ptr) {
    auto* size = fromJavaPointer<mbgl::Size>(ptr);
    return static_cast<jint>(size->width * size->height);
}

JNIEXPORT jfloat JNICALL Java_com_maplibre_jni_Size_nativeAspectRatio
  (JNIEnv* env, jclass, jlong ptr) {
    auto* size = fromJavaPointer<mbgl::Size>(ptr);
    if (size->height == 0) {
        return 0.0f;
    }
    return static_cast<jfloat>(size->width) / static_cast<jfloat>(size->height);
}

JNIEXPORT jboolean JNICALL Java_com_maplibre_jni_Size_nativeIsEmpty
  (JNIEnv* env, jclass, jlong ptr) {
    auto* size = fromJavaPointer<mbgl::Size>(ptr);
    return size->width == 0 || size->height == 0;
}

JNIEXPORT jboolean JNICALL Java_com_maplibre_jni_Size_nativeEquals
  (JNIEnv* env, jclass, jlong ptr, jlong otherPtr) {
    auto* size = fromJavaPointer<mbgl::Size>(ptr);
    auto* other = fromJavaPointer<mbgl::Size>(otherPtr);
    return *size == *other;
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_Size_nativeDestroy
  (JNIEnv* env, jclass, jlong ptr) {
    delete fromJavaPointer<mbgl::Size>(ptr);
}

}