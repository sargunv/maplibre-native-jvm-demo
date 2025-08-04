#include "generated/kotlin/main/com_maplibre_jni_CameraOptions.h"
#include "mbgl/map/camera.hpp"
#include "jni_helpers.hpp"
#include "conversions/jni_latlng_conversions.hpp"
#include "conversions/jni_screencoordinate_conversions.hpp"
#include "conversions/jni_edgeinsets_conversions.hpp"

extern "C" {

JNIEXPORT jlong JNICALL Java_com_maplibre_jni_CameraOptions_nativeNew
  (JNIEnv* env, jclass) {
    auto* options = new mbgl::CameraOptions();
    return toJavaPointer(options);
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_CameraOptions_nativeSetCenter
  (JNIEnv* env, jclass, jlong ptr, jobject center) {
    auto* options = fromJavaPointer<mbgl::CameraOptions>(ptr);
    if (center == nullptr) {
        options->center = std::nullopt;
    } else {
        options->center = maplibre_jni::LatLngConversions::extract(env, center);
    }
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_CameraOptions_nativeSetPadding
  (JNIEnv* env, jclass, jlong ptr, jobject padding) {
    auto* options = fromJavaPointer<mbgl::CameraOptions>(ptr);
    if (padding == nullptr) {
        options->padding = std::nullopt;
    } else {
        options->padding = maplibre_jni::EdgeInsetsConversions::extract(env, padding);
    }
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_CameraOptions_nativeSetAnchor
  (JNIEnv* env, jclass, jlong ptr, jobject anchor) {
    auto* options = fromJavaPointer<mbgl::CameraOptions>(ptr);
    if (anchor == nullptr) {
        options->anchor = std::nullopt;
    } else {
        options->anchor = maplibre_jni::ScreenCoordinateConversions::extract(env, anchor);
    }
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_CameraOptions_nativeSetZoomOptional
  (JNIEnv* env, jclass, jlong ptr, jdouble zoom, jboolean hasValue) {
    auto* options = fromJavaPointer<mbgl::CameraOptions>(ptr);
    if (hasValue) {
        options->zoom = zoom;
    } else {
        options->zoom = std::nullopt;
    }
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_CameraOptions_nativeSetBearingOptional
  (JNIEnv* env, jclass, jlong ptr, jdouble bearing, jboolean hasValue) {
    auto* options = fromJavaPointer<mbgl::CameraOptions>(ptr);
    if (hasValue) {
        options->bearing = bearing;
    } else {
        options->bearing = std::nullopt;
    }
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_CameraOptions_nativeSetPitchOptional
  (JNIEnv* env, jclass, jlong ptr, jdouble pitch, jboolean hasValue) {
    auto* options = fromJavaPointer<mbgl::CameraOptions>(ptr);
    if (hasValue) {
        options->pitch = pitch;
    } else {
        options->pitch = std::nullopt;
    }
}

JNIEXPORT jboolean JNICALL Java_com_maplibre_jni_CameraOptions_nativeEquals
  (JNIEnv* env, jclass, jlong ptr, jlong otherPtr) {
    auto* options = fromJavaPointer<mbgl::CameraOptions>(ptr);
    auto* other = fromJavaPointer<mbgl::CameraOptions>(otherPtr);
    return *options == *other;
}

JNIEXPORT jobject JNICALL Java_com_maplibre_jni_CameraOptions_nativeGetCenter
  (JNIEnv* env, jclass, jlong ptr) {
    auto* options = fromJavaPointer<mbgl::CameraOptions>(ptr);
    if (options->center.has_value()) {
        return maplibre_jni::LatLngConversions::create(env, options->center.value());
    }
    return nullptr;
}

JNIEXPORT jobject JNICALL Java_com_maplibre_jni_CameraOptions_nativeGetPadding
  (JNIEnv* env, jclass, jlong ptr) {
    auto* options = fromJavaPointer<mbgl::CameraOptions>(ptr);
    if (options->padding.has_value()) {
        return maplibre_jni::EdgeInsetsConversions::create(env, options->padding.value());
    }
    return nullptr;
}

JNIEXPORT jobject JNICALL Java_com_maplibre_jni_CameraOptions_nativeGetAnchor
  (JNIEnv* env, jclass, jlong ptr) {
    auto* options = fromJavaPointer<mbgl::CameraOptions>(ptr);
    if (options->anchor.has_value()) {
        return maplibre_jni::ScreenCoordinateConversions::create(env, options->anchor.value());
    }
    return nullptr;
}

JNIEXPORT jobject JNICALL Java_com_maplibre_jni_CameraOptions_nativeGetZoomOptional
  (JNIEnv* env, jclass, jlong ptr) {
    auto* options = fromJavaPointer<mbgl::CameraOptions>(ptr);
    if (options->zoom.has_value()) {
        jclass doubleClass = env->FindClass("java/lang/Double");
        jmethodID valueOf = env->GetStaticMethodID(doubleClass, "valueOf", "(D)Ljava/lang/Double;");
        return env->CallStaticObjectMethod(doubleClass, valueOf, options->zoom.value());
    }
    return nullptr;
}

JNIEXPORT jobject JNICALL Java_com_maplibre_jni_CameraOptions_nativeGetBearingOptional
  (JNIEnv* env, jclass, jlong ptr) {
    auto* options = fromJavaPointer<mbgl::CameraOptions>(ptr);
    if (options->bearing.has_value()) {
        jclass doubleClass = env->FindClass("java/lang/Double");
        jmethodID valueOf = env->GetStaticMethodID(doubleClass, "valueOf", "(D)Ljava/lang/Double;");
        return env->CallStaticObjectMethod(doubleClass, valueOf, options->bearing.value());
    }
    return nullptr;
}

JNIEXPORT jobject JNICALL Java_com_maplibre_jni_CameraOptions_nativeGetPitchOptional
  (JNIEnv* env, jclass, jlong ptr) {
    auto* options = fromJavaPointer<mbgl::CameraOptions>(ptr);
    if (options->pitch.has_value()) {
        jclass doubleClass = env->FindClass("java/lang/Double");
        jmethodID valueOf = env->GetStaticMethodID(doubleClass, "valueOf", "(D)Ljava/lang/Double;");
        return env->CallStaticObjectMethod(doubleClass, valueOf, options->pitch.value());
    }
    return nullptr;
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_CameraOptions_nativeDestroy
  (JNIEnv* env, jclass, jlong ptr) {
    delete fromJavaPointer<mbgl::CameraOptions>(ptr);
}

}