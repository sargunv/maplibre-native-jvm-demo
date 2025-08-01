#include "generated/kotlin/main/com_maplibre_jni_ScreenCoordinate.h"
#include "mbgl/util/geo.hpp"
#include "jni_helpers.hpp"

extern "C" {

JNIEXPORT jlong JNICALL Java_com_maplibre_jni_ScreenCoordinate_nativeNew
  (JNIEnv* env, jclass, jdouble x, jdouble y) {
    auto* coord = new mbgl::ScreenCoordinate(x, y);
    return toJavaPointer(coord);
}

JNIEXPORT jdouble JNICALL Java_com_maplibre_jni_ScreenCoordinate_nativeX
  (JNIEnv* env, jclass, jlong ptr) {
    auto* coord = fromJavaPointer<mbgl::ScreenCoordinate>(ptr);
    return coord->x;
}

JNIEXPORT jdouble JNICALL Java_com_maplibre_jni_ScreenCoordinate_nativeY
  (JNIEnv* env, jclass, jlong ptr) {
    auto* coord = fromJavaPointer<mbgl::ScreenCoordinate>(ptr);
    return coord->y;
}

JNIEXPORT jboolean JNICALL Java_com_maplibre_jni_ScreenCoordinate_nativeEquals
  (JNIEnv* env, jclass, jlong ptr, jlong otherPtr) {
    auto* coord = fromJavaPointer<mbgl::ScreenCoordinate>(ptr);
    auto* other = fromJavaPointer<mbgl::ScreenCoordinate>(otherPtr);
    return *coord == *other;
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_ScreenCoordinate_nativeDestroy
  (JNIEnv* env, jclass, jlong ptr) {
    delete fromJavaPointer<mbgl::ScreenCoordinate>(ptr);
}

}