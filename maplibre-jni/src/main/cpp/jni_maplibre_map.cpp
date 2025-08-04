#include "com_maplibre_jni_MaplibreMap.h"
#include "jni_helpers.hpp"
#include "jni_awt_canvas_renderer.hpp"
#include "jni_map_observer.hpp"
#include "conversions/jni_size_conversions.hpp"
#include "conversions/jni_cameraoptions_conversions.hpp"
#include "conversions/jni_mapoptions_conversions.hpp"
#include "conversions/jni_clientoptions_conversions.hpp"
#include "conversions/jni_resourceoptions_conversions.hpp"
#include <mbgl/map/map.hpp>
#include <mbgl/map/map_options.hpp>
#include <mbgl/storage/resource_options.hpp>
#include <mbgl/util/client_options.hpp>
#include <mbgl/map/camera.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/util/size.hpp>
#include <mbgl/annotation/annotation.hpp>
#include <mbgl/storage/file_source_manager.hpp>
#include <mbgl/storage/database_file_source.hpp>

extern "C" {

JNIEXPORT jlong JNICALL Java_com_maplibre_jni_MaplibreMap_nativeNew
  (JNIEnv* env, jclass, jlong frontendPtr, jlong observerPtr, jobject mapOptionsObj, jobject resourceOptionsObj, jobject clientOptionsObj) {
    try {
        auto* frontend = fromJavaPointer<maplibre_jni::AwtCanvasRenderer>(frontendPtr);
        auto* observer = fromJavaPointer<maplibre_jni::JniMapObserver>(observerPtr);
        
        // Extract MapOptions from Java object
        mbgl::MapOptions mapOptions = maplibre_jni::MapOptionsConversions::extract(env, mapOptionsObj);
        
        // Extract ResourceOptions from Java object
        mbgl::ResourceOptions resourceOptions = maplibre_jni::ResourceOptionsConversions::extract(env, resourceOptionsObj);
        
        // Extract ClientOptions from Java object
        mbgl::ClientOptions clientOptions = maplibre_jni::ClientOptionsConversions::extract(env, clientOptionsObj);
        
        auto* map = new mbgl::Map(
            *frontend,
            *observer,
            mapOptions,
            resourceOptions,
            clientOptions
        );
        
        return toJavaPointer(map);
    } catch (const std::exception& e) {
        throwJavaException(env, "java/lang/RuntimeException", e.what());
        return 0;
    }
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_MaplibreMap_nativeDestroy
  (JNIEnv* env, jclass, jlong ptr) {
    delete fromJavaPointer<mbgl::Map>(ptr);
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_MaplibreMap_nativeTriggerRepaint
  (JNIEnv* env, jclass, jlong ptr) {
    auto* map = fromJavaPointer<mbgl::Map>(ptr);
    map->triggerRepaint();
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_MaplibreMap_nativeLoadStyleURL
  (JNIEnv* env, jclass, jlong ptr, jstring jUrl) {
    auto* map = fromJavaPointer<mbgl::Map>(ptr);
    const char* url = env->GetStringUTFChars(jUrl, nullptr);
    map->getStyle().loadURL(std::string(url));
    env->ReleaseStringUTFChars(jUrl, url);
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_MaplibreMap_nativeLoadStyleJSON
  (JNIEnv* env, jclass, jlong ptr, jstring jJson) {
    auto* map = fromJavaPointer<mbgl::Map>(ptr);
    const char* json = env->GetStringUTFChars(jJson, nullptr);
    map->getStyle().loadJSON(std::string(json));
    env->ReleaseStringUTFChars(jJson, json);
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_MaplibreMap_nativeJumpTo
  (JNIEnv* env, jclass, jlong ptr, jobject cameraOptions) {
    auto* map = fromJavaPointer<mbgl::Map>(ptr);
    mbgl::CameraOptions options = maplibre_jni::CameraOptionsConversions::extract(env, cameraOptions);
    map->jumpTo(options);
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_MaplibreMap_nativeEaseTo
  (JNIEnv* env, jclass, jlong ptr, jobject cameraOptions, jint duration) {
    auto* map = fromJavaPointer<mbgl::Map>(ptr);
    mbgl::CameraOptions options = maplibre_jni::CameraOptionsConversions::extract(env, cameraOptions);
    
    mbgl::AnimationOptions animationOptions;
    animationOptions.duration = mbgl::Duration(std::chrono::milliseconds(duration));
    
    map->easeTo(options, animationOptions);
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_MaplibreMap_nativeFlyTo
  (JNIEnv* env, jclass, jlong ptr, jobject cameraOptions, jint duration) {
    auto* map = fromJavaPointer<mbgl::Map>(ptr);
    mbgl::CameraOptions options = maplibre_jni::CameraOptionsConversions::extract(env, cameraOptions);
    
    mbgl::AnimationOptions animationOptions;
    animationOptions.duration = mbgl::Duration(std::chrono::milliseconds(duration));
    
    map->flyTo(options, animationOptions);
}

JNIEXPORT jobject JNICALL Java_com_maplibre_jni_MaplibreMap_nativeGetCameraOptions
  (JNIEnv* env, jclass, jlong ptr) {
    auto* map = fromJavaPointer<mbgl::Map>(ptr);
    auto cameraOptions = map->getCameraOptions();
    return maplibre_jni::CameraOptionsConversions::create(env, cameraOptions);
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_MaplibreMap_nativeSetSize
  (JNIEnv* env, jclass, jlong ptr, jobject size) {
    auto* map = fromJavaPointer<mbgl::Map>(ptr);
    mbgl::Size mbglSize = maplibre_jni::SizeConversions::extract(env, size);
    map->setSize(mbglSize);
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_MaplibreMap_nativeActivateFileSources
  (JNIEnv* env, jclass, jobject resourceOptionsObj, jobject clientOptionsObj) {
    try {
        // Extract ResourceOptions from Java object
        mbgl::ResourceOptions resourceOptions = maplibre_jni::ResourceOptionsConversions::extract(env, resourceOptionsObj);
        
        // Extract ClientOptions from Java object
        mbgl::ClientOptions clientOptions = maplibre_jni::ClientOptionsConversions::extract(env, clientOptionsObj);
        
        // Get network file source for HTTP downloads
        std::shared_ptr<mbgl::FileSource> networkFileSource = 
            mbgl::FileSourceManager::get()->getFileSource(
                mbgl::FileSourceType::Network, resourceOptions, clientOptions);
        
        
        // Get resource loader for request management
        std::shared_ptr<mbgl::FileSource> resourceLoader = 
            mbgl::FileSourceManager::get()->getFileSource(
                mbgl::FileSourceType::ResourceLoader, resourceOptions, clientOptions);
        
        
        // Get database file source for caching
        std::shared_ptr<mbgl::FileSource> databaseFileSource = 
            mbgl::FileSourceManager::get()->getFileSource(
                mbgl::FileSourceType::Database, resourceOptions, clientOptions);
        
        
    } catch (const std::exception& e) {
        throwJavaException(env, "java/lang/RuntimeException", e.what());
    }
}

}