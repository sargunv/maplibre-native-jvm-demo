#include "generated/kotlin/main/com_maplibre_jni_MapOptions.h"
#include "mbgl/map/map_options.hpp"
#include "mbgl/map/mode.hpp"
#include "mbgl/util/geo.hpp"
#include "mbgl/util/size.hpp"
#include "jni_helpers.hpp"
#include "conversions/jni_size_conversions.hpp"

extern "C" {

JNIEXPORT jlong JNICALL Java_com_maplibre_jni_MapOptions_nativeNew
  (JNIEnv* env, jclass) {
    auto* options = new mbgl::MapOptions();
    return toJavaPointer(options);
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_MapOptions_nativeDestroy
  (JNIEnv* env, jclass, jlong ptr) {
    delete fromJavaPointer<mbgl::MapOptions>(ptr);
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_MapOptions_nativeSetMapMode
  (JNIEnv* env, jclass, jlong ptr, jint mode) {
    auto* options = fromJavaPointer<mbgl::MapOptions>(ptr);
    options->withMapMode(static_cast<mbgl::MapMode>(mode));
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_MapOptions_nativeSetConstrainMode
  (JNIEnv* env, jclass, jlong ptr, jint mode) {
    auto* options = fromJavaPointer<mbgl::MapOptions>(ptr);
    options->withConstrainMode(static_cast<mbgl::ConstrainMode>(mode));
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_MapOptions_nativeSetViewportMode
  (JNIEnv* env, jclass, jlong ptr, jint mode) {
    auto* options = fromJavaPointer<mbgl::MapOptions>(ptr);
    options->withViewportMode(static_cast<mbgl::ViewportMode>(mode));
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_MapOptions_nativeSetCrossSourceCollisions
  (JNIEnv* env, jclass, jlong ptr, jboolean enabled) {
    auto* options = fromJavaPointer<mbgl::MapOptions>(ptr);
    options->withCrossSourceCollisions(enabled);
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_MapOptions_nativeSetNorthOrientation
  (JNIEnv* env, jclass, jlong ptr, jint orientation) {
    auto* options = fromJavaPointer<mbgl::MapOptions>(ptr);
    options->withNorthOrientation(static_cast<mbgl::NorthOrientation>(orientation));
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_MapOptions_nativeSetSize
  (JNIEnv* env, jclass, jlong ptr, jobject size) {
    auto* options = fromJavaPointer<mbgl::MapOptions>(ptr);
    mbgl::Size mbglSize = maplibre_jni::SizeConversions::extract(env, size);
    options->withSize(mbglSize);
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_MapOptions_nativeSetPixelRatio
  (JNIEnv* env, jclass, jlong ptr, jfloat ratio) {
    auto* options = fromJavaPointer<mbgl::MapOptions>(ptr);
    options->withPixelRatio(ratio);
}

JNIEXPORT jint JNICALL Java_com_maplibre_jni_MapOptions_nativeGetMapMode
  (JNIEnv* env, jclass, jlong ptr) {
    auto* options = fromJavaPointer<mbgl::MapOptions>(ptr);
    return static_cast<jint>(options->mapMode());
}

JNIEXPORT jint JNICALL Java_com_maplibre_jni_MapOptions_nativeGetConstrainMode
  (JNIEnv* env, jclass, jlong ptr) {
    auto* options = fromJavaPointer<mbgl::MapOptions>(ptr);
    return static_cast<jint>(options->constrainMode());
}

JNIEXPORT jint JNICALL Java_com_maplibre_jni_MapOptions_nativeGetViewportMode
  (JNIEnv* env, jclass, jlong ptr) {
    auto* options = fromJavaPointer<mbgl::MapOptions>(ptr);
    return static_cast<jint>(options->viewportMode());
}

JNIEXPORT jboolean JNICALL Java_com_maplibre_jni_MapOptions_nativeGetCrossSourceCollisions
  (JNIEnv* env, jclass, jlong ptr) {
    auto* options = fromJavaPointer<mbgl::MapOptions>(ptr);
    return options->crossSourceCollisions();
}

JNIEXPORT jint JNICALL Java_com_maplibre_jni_MapOptions_nativeGetNorthOrientation
  (JNIEnv* env, jclass, jlong ptr) {
    auto* options = fromJavaPointer<mbgl::MapOptions>(ptr);
    return static_cast<jint>(options->northOrientation());
}

JNIEXPORT jobject JNICALL Java_com_maplibre_jni_MapOptions_nativeGetSize
  (JNIEnv* env, jclass, jlong ptr) {
    auto* options = fromJavaPointer<mbgl::MapOptions>(ptr);
    return maplibre_jni::SizeConversions::create(env, options->size());
}

JNIEXPORT jfloat JNICALL Java_com_maplibre_jni_MapOptions_nativeGetPixelRatio
  (JNIEnv* env, jclass, jlong ptr) {
    auto* options = fromJavaPointer<mbgl::MapOptions>(ptr);
    return options->pixelRatio();
}

}