#include "com_maplibre_jni_ClientOptions.h"
#include "mbgl/util/client_options.hpp"
#include "jni_helpers.hpp"

extern "C" {

JNIEXPORT jlong JNICALL Java_com_maplibre_jni_ClientOptions_nativeNew
  (JNIEnv* env, jclass) {
    auto* options = new mbgl::ClientOptions();
    return toJavaPointer(options);
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_ClientOptions_nativeDestroy
  (JNIEnv* env, jclass, jlong ptr) {
    delete fromJavaPointer<mbgl::ClientOptions>(ptr);
}

JNIEXPORT jlong JNICALL Java_com_maplibre_jni_ClientOptions_nativeClone
  (JNIEnv* env, jclass, jlong ptr) {
    auto* options = fromJavaPointer<mbgl::ClientOptions>(ptr);
    auto* cloned = new mbgl::ClientOptions(options->clone());
    return toJavaPointer(cloned);
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_ClientOptions_nativeSetName
  (JNIEnv* env, jclass, jlong ptr, jstring name) {
    auto* options = fromJavaPointer<mbgl::ClientOptions>(ptr);
    const char* nameStr = env->GetStringUTFChars(name, nullptr);
    options->withName(std::string(nameStr));
    env->ReleaseStringUTFChars(name, nameStr);
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_ClientOptions_nativeSetVersion
  (JNIEnv* env, jclass, jlong ptr, jstring version) {
    auto* options = fromJavaPointer<mbgl::ClientOptions>(ptr);
    const char* versionStr = env->GetStringUTFChars(version, nullptr);
    options->withVersion(std::string(versionStr));
    env->ReleaseStringUTFChars(version, versionStr);
}

JNIEXPORT jstring JNICALL Java_com_maplibre_jni_ClientOptions_nativeGetName
  (JNIEnv* env, jclass, jlong ptr) {
    auto* options = fromJavaPointer<mbgl::ClientOptions>(ptr);
    return env->NewStringUTF(options->name().c_str());
}

JNIEXPORT jstring JNICALL Java_com_maplibre_jni_ClientOptions_nativeGetVersion
  (JNIEnv* env, jclass, jlong ptr) {
    auto* options = fromJavaPointer<mbgl::ClientOptions>(ptr);
    return env->NewStringUTF(options->version().c_str());
}

}