#include "generated/kotlin/main/com_maplibre_jni_EdgeInsets.h"
#include "mbgl/util/geo.hpp"
#include "jni_helpers.hpp"

extern "C" {

JNIEXPORT jlong JNICALL Java_com_maplibre_jni_EdgeInsets_nativeNew
  (JNIEnv* env, jclass, jdouble top, jdouble left, jdouble bottom, jdouble right) {
    auto* insets = new mbgl::EdgeInsets(top, left, bottom, right);
    return toJavaPointer(insets);
}

JNIEXPORT jdouble JNICALL Java_com_maplibre_jni_EdgeInsets_nativeTop
  (JNIEnv* env, jclass, jlong ptr) {
    auto* insets = fromJavaPointer<mbgl::EdgeInsets>(ptr);
    return insets->top();
}

JNIEXPORT jdouble JNICALL Java_com_maplibre_jni_EdgeInsets_nativeLeft
  (JNIEnv* env, jclass, jlong ptr) {
    auto* insets = fromJavaPointer<mbgl::EdgeInsets>(ptr);
    return insets->left();
}

JNIEXPORT jdouble JNICALL Java_com_maplibre_jni_EdgeInsets_nativeBottom
  (JNIEnv* env, jclass, jlong ptr) {
    auto* insets = fromJavaPointer<mbgl::EdgeInsets>(ptr);
    return insets->bottom();
}

JNIEXPORT jdouble JNICALL Java_com_maplibre_jni_EdgeInsets_nativeRight
  (JNIEnv* env, jclass, jlong ptr) {
    auto* insets = fromJavaPointer<mbgl::EdgeInsets>(ptr);
    return insets->right();
}

JNIEXPORT jboolean JNICALL Java_com_maplibre_jni_EdgeInsets_nativeIsFlush
  (JNIEnv* env, jclass, jlong ptr) {
    auto* insets = fromJavaPointer<mbgl::EdgeInsets>(ptr);
    return insets->isFlush();
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_EdgeInsets_nativePlusAssign
  (JNIEnv* env, jclass, jlong ptr, jlong otherPtr) {
    auto* insets = fromJavaPointer<mbgl::EdgeInsets>(ptr);
    auto* other = fromJavaPointer<mbgl::EdgeInsets>(otherPtr);
    *insets += *other;
}

JNIEXPORT jlong JNICALL Java_com_maplibre_jni_EdgeInsets_nativeGetCenter
  (JNIEnv* env, jclass, jlong ptr, jint width, jint height) {
    auto* insets = fromJavaPointer<mbgl::EdgeInsets>(ptr);
    auto center = insets->getCenter(static_cast<uint16_t>(width), static_cast<uint16_t>(height));
    auto* screenCoord = new mbgl::ScreenCoordinate(center);
    return toJavaPointer(screenCoord);
}

JNIEXPORT jboolean JNICALL Java_com_maplibre_jni_EdgeInsets_nativeEquals
  (JNIEnv* env, jclass, jlong ptr, jlong otherPtr) {
    auto* insets = fromJavaPointer<mbgl::EdgeInsets>(ptr);
    auto* other = fromJavaPointer<mbgl::EdgeInsets>(otherPtr);
    return *insets == *other;
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_EdgeInsets_nativeDestroy
  (JNIEnv* env, jclass, jlong ptr) {
    delete fromJavaPointer<mbgl::EdgeInsets>(ptr);
}

}