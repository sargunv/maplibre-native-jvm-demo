#include "generated/kotlin/main/com_maplibre_jni_LatLng.h"
#include "mbgl/util/geo.hpp"
#include "jni_helpers.hpp"

extern "C" {

JNIEXPORT jlong JNICALL Java_com_maplibre_jni_LatLng_nativeNew
  (JNIEnv* env, jclass, jdouble latitude, jdouble longitude, jboolean wrap) {
    auto* latlng = new mbgl::LatLng(latitude, longitude, wrap ? mbgl::LatLng::Wrapped : mbgl::LatLng::Unwrapped);
    return toJavaPointer(latlng);
}

JNIEXPORT jdouble JNICALL Java_com_maplibre_jni_LatLng_nativeLatitude
  (JNIEnv* env, jclass, jlong ptr) {
    auto* latlng = fromJavaPointer<mbgl::LatLng>(ptr);
    return latlng->latitude();
}

JNIEXPORT jdouble JNICALL Java_com_maplibre_jni_LatLng_nativeLongitude
  (JNIEnv* env, jclass, jlong ptr) {
    auto* latlng = fromJavaPointer<mbgl::LatLng>(ptr);
    return latlng->longitude();
}

JNIEXPORT jlong JNICALL Java_com_maplibre_jni_LatLng_nativeWrapped
  (JNIEnv* env, jclass, jlong ptr) {
    auto* latlng = fromJavaPointer<mbgl::LatLng>(ptr);
    auto* wrapped = new mbgl::LatLng(latlng->wrapped());
    return toJavaPointer(wrapped);
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_LatLng_nativeWrap
  (JNIEnv* env, jclass, jlong ptr) {
    auto* latlng = fromJavaPointer<mbgl::LatLng>(ptr);
    latlng->wrap();
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_LatLng_nativeUnwrapForShortestPath
  (JNIEnv* env, jclass, jlong ptr, jlong endPtr) {
    auto* latlng = fromJavaPointer<mbgl::LatLng>(ptr);
    auto* end = fromJavaPointer<mbgl::LatLng>(endPtr);
    latlng->unwrapForShortestPath(*end);
}

JNIEXPORT jboolean JNICALL Java_com_maplibre_jni_LatLng_nativeEquals
  (JNIEnv* env, jclass, jlong ptr, jlong otherPtr) {
    auto* latlng = fromJavaPointer<mbgl::LatLng>(ptr);
    auto* other = fromJavaPointer<mbgl::LatLng>(otherPtr);
    return *latlng == *other;
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_LatLng_nativeDestroy
  (JNIEnv* env, jclass, jlong ptr) {
    delete fromJavaPointer<mbgl::LatLng>(ptr);
}

}