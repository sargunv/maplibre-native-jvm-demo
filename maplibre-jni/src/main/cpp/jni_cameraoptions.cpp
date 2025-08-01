#include "generated/kotlin/main/com_maplibre_jni_CameraOptions.h"
#include "mbgl/map/camera.hpp"
#include "jni_helpers.hpp"

extern "C" {

JNIEXPORT jlong JNICALL Java_com_maplibre_jni_CameraOptions_nativeNew
  (JNIEnv* env, jclass) {
    auto* options = new mbgl::CameraOptions();
    return toJavaPointer(options);
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_CameraOptions_nativeSetCenter
  (JNIEnv* env, jclass, jlong ptr, jlong centerPtr) {
    auto* options = fromJavaPointer<mbgl::CameraOptions>(ptr);
    if (centerPtr == 0) {
        options->center = std::nullopt;
    } else {
        options->center = *fromJavaPointer<mbgl::LatLng>(centerPtr);
    }
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_CameraOptions_nativeSetPadding
  (JNIEnv* env, jclass, jlong ptr, jlong paddingPtr) {
    auto* options = fromJavaPointer<mbgl::CameraOptions>(ptr);
    if (paddingPtr == 0) {
        options->padding = std::nullopt;
    } else {
        options->padding = *fromJavaPointer<mbgl::EdgeInsets>(paddingPtr);
    }
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_CameraOptions_nativeSetAnchor
  (JNIEnv* env, jclass, jlong ptr, jlong anchorPtr) {
    auto* options = fromJavaPointer<mbgl::CameraOptions>(ptr);
    if (anchorPtr == 0) {
        options->anchor = std::nullopt;
    } else {
        options->anchor = *fromJavaPointer<mbgl::ScreenCoordinate>(anchorPtr);
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

JNIEXPORT jlong JNICALL Java_com_maplibre_jni_CameraOptions_nativeGetCenter
  (JNIEnv* env, jclass, jlong ptr) {
    auto* options = fromJavaPointer<mbgl::CameraOptions>(ptr);
    if (options->center.has_value()) {
        return toJavaPointer(new mbgl::LatLng(options->center.value()));
    }
    return 0;
}

JNIEXPORT jlong JNICALL Java_com_maplibre_jni_CameraOptions_nativeGetPadding
  (JNIEnv* env, jclass, jlong ptr) {
    auto* options = fromJavaPointer<mbgl::CameraOptions>(ptr);
    if (options->padding.has_value()) {
        return toJavaPointer(new mbgl::EdgeInsets(options->padding.value()));
    }
    return 0;
}

JNIEXPORT jlong JNICALL Java_com_maplibre_jni_CameraOptions_nativeGetAnchor
  (JNIEnv* env, jclass, jlong ptr) {
    auto* options = fromJavaPointer<mbgl::CameraOptions>(ptr);
    if (options->anchor.has_value()) {
        return toJavaPointer(new mbgl::ScreenCoordinate(options->anchor.value()));
    }
    return 0;
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