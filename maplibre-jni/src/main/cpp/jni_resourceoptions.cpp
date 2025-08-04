#include "com_maplibre_jni_ResourceOptions.h"
#include "mbgl/storage/resource_options.hpp"
#include "jni_helpers.hpp"

extern "C" {

JNIEXPORT jlong JNICALL Java_com_maplibre_jni_ResourceOptions_nativeDefault
  (JNIEnv* env, jclass) {
    auto* options = new mbgl::ResourceOptions(mbgl::ResourceOptions::Default());
    return toJavaPointer(options);
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_ResourceOptions_nativeDestroy
  (JNIEnv* env, jclass, jlong ptr) {
    delete fromJavaPointer<mbgl::ResourceOptions>(ptr);
}

JNIEXPORT jlong JNICALL Java_com_maplibre_jni_ResourceOptions_nativeClone
  (JNIEnv* env, jclass, jlong ptr) {
    auto* options = fromJavaPointer<mbgl::ResourceOptions>(ptr);
    auto* cloned = new mbgl::ResourceOptions(options->clone());
    return toJavaPointer(cloned);
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_ResourceOptions_nativeSetApiKey
  (JNIEnv* env, jclass, jlong ptr, jstring apiKey) {
    auto* options = fromJavaPointer<mbgl::ResourceOptions>(ptr);
    const char* apiKeyStr = env->GetStringUTFChars(apiKey, nullptr);
    options->withApiKey(std::string(apiKeyStr));
    env->ReleaseStringUTFChars(apiKey, apiKeyStr);
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_ResourceOptions_nativeSetCachePath
  (JNIEnv* env, jclass, jlong ptr, jstring path) {
    auto* options = fromJavaPointer<mbgl::ResourceOptions>(ptr);
    const char* pathStr = env->GetStringUTFChars(path, nullptr);
    options->withCachePath(std::string(pathStr));
    env->ReleaseStringUTFChars(path, pathStr);
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_ResourceOptions_nativeSetAssetPath
  (JNIEnv* env, jclass, jlong ptr, jstring path) {
    auto* options = fromJavaPointer<mbgl::ResourceOptions>(ptr);
    const char* pathStr = env->GetStringUTFChars(path, nullptr);
    options->withAssetPath(std::string(pathStr));
    env->ReleaseStringUTFChars(path, pathStr);
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_ResourceOptions_nativeSetMaximumCacheSize
  (JNIEnv* env, jclass, jlong ptr, jlong size) {
    auto* options = fromJavaPointer<mbgl::ResourceOptions>(ptr);
    options->withMaximumCacheSize(static_cast<uint64_t>(size));
}

JNIEXPORT jstring JNICALL Java_com_maplibre_jni_ResourceOptions_nativeGetApiKey
  (JNIEnv* env, jclass, jlong ptr) {
    auto* options = fromJavaPointer<mbgl::ResourceOptions>(ptr);
    return env->NewStringUTF(options->apiKey().c_str());
}

JNIEXPORT jstring JNICALL Java_com_maplibre_jni_ResourceOptions_nativeGetCachePath
  (JNIEnv* env, jclass, jlong ptr) {
    auto* options = fromJavaPointer<mbgl::ResourceOptions>(ptr);
    return env->NewStringUTF(options->cachePath().c_str());
}

JNIEXPORT jstring JNICALL Java_com_maplibre_jni_ResourceOptions_nativeGetAssetPath
  (JNIEnv* env, jclass, jlong ptr) {
    auto* options = fromJavaPointer<mbgl::ResourceOptions>(ptr);
    return env->NewStringUTF(options->assetPath().c_str());
}

JNIEXPORT jlong JNICALL Java_com_maplibre_jni_ResourceOptions_nativeGetMaximumCacheSize
  (JNIEnv* env, jclass, jlong ptr) {
    auto* options = fromJavaPointer<mbgl::ResourceOptions>(ptr);
    return static_cast<jlong>(options->maximumCacheSize());
}

// TODO: Implement TileServerOptions getter when TileServerOptions wrapper is available
// TODO: Implement platformContext getter - platform-specific handling required

}