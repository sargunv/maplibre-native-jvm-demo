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
#include <memory>

// Wrapper struct to manage objects whose lifetime must match the Map's lifetime
struct MapWrapper {
    std::unique_ptr<mbgl::Map> map;
    std::unique_ptr<maplibre_jni::JniMapObserver> observer;
    // Future: could add renderer frontend, file sources, etc.
    
    MapWrapper(mbgl::Map* m, maplibre_jni::JniMapObserver* o) 
        : map(m), observer(o) {}
};

extern "C" {

JNIEXPORT jlong JNICALL Java_com_maplibre_jni_MaplibreMap_nativeNew
  (JNIEnv* env, jclass, jlong frontendPtr, jobject mapObserverObj, jobject mapOptionsObj, jobject resourceOptionsObj, jobject clientOptionsObj) {
    try {
        auto* frontend = fromJavaPointer<maplibre_jni::AwtCanvasRenderer>(frontendPtr);
        
        // Create the JniMapObserver from the Java MapObserver object
        auto* observer = new maplibre_jni::JniMapObserver(env, mapObserverObj);
        
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
        
        // Create wrapper to manage both map and observer lifetime
        auto* wrapper = new MapWrapper(map, observer);
        
        return toJavaPointer(wrapper);
    } catch (const std::exception& e) {
        throwJavaException(env, "java/lang/RuntimeException", e.what());
        return 0;
    }
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_MaplibreMap_nativeDestroy
  (JNIEnv* env, jclass, jlong ptr) {
    // Deleting the wrapper will automatically delete both map and observer via unique_ptr
    delete fromJavaPointer<MapWrapper>(ptr);
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_MaplibreMap_nativeTriggerRepaint
  (JNIEnv* env, jclass, jlong ptr) {
    auto* wrapper = fromJavaPointer<MapWrapper>(ptr);
    wrapper->map->triggerRepaint();
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_MaplibreMap_nativeLoadStyleURL
  (JNIEnv* env, jclass, jlong ptr, jstring jUrl) {
    auto* wrapper = fromJavaPointer<MapWrapper>(ptr);
    const char* url = env->GetStringUTFChars(jUrl, nullptr);
    wrapper->map->getStyle().loadURL(std::string(url));
    env->ReleaseStringUTFChars(jUrl, url);
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_MaplibreMap_nativeLoadStyleJSON
  (JNIEnv* env, jclass, jlong ptr, jstring jJson) {
    auto* wrapper = fromJavaPointer<MapWrapper>(ptr);
    const char* json = env->GetStringUTFChars(jJson, nullptr);
    wrapper->map->getStyle().loadJSON(std::string(json));
    env->ReleaseStringUTFChars(jJson, json);
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_MaplibreMap_nativeJumpTo
  (JNIEnv* env, jclass, jlong ptr, jobject cameraOptions) {
    auto* wrapper = fromJavaPointer<MapWrapper>(ptr);
    mbgl::CameraOptions options = maplibre_jni::CameraOptionsConversions::extract(env, cameraOptions);
    wrapper->map->jumpTo(options);
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_MaplibreMap_nativeEaseTo
  (JNIEnv* env, jclass, jlong ptr, jobject cameraOptions, jint duration) {
    auto* wrapper = fromJavaPointer<MapWrapper>(ptr);
    mbgl::CameraOptions options = maplibre_jni::CameraOptionsConversions::extract(env, cameraOptions);
    
    mbgl::AnimationOptions animationOptions;
    animationOptions.duration = mbgl::Duration(std::chrono::milliseconds(duration));
    
    wrapper->map->easeTo(options, animationOptions);
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_MaplibreMap_nativeFlyTo
  (JNIEnv* env, jclass, jlong ptr, jobject cameraOptions, jint duration) {
    auto* wrapper = fromJavaPointer<MapWrapper>(ptr);
    mbgl::CameraOptions options = maplibre_jni::CameraOptionsConversions::extract(env, cameraOptions);
    
    mbgl::AnimationOptions animationOptions;
    animationOptions.duration = mbgl::Duration(std::chrono::milliseconds(duration));
    
    wrapper->map->flyTo(options, animationOptions);
}

JNIEXPORT jobject JNICALL Java_com_maplibre_jni_MaplibreMap_nativeGetCameraOptions
  (JNIEnv* env, jclass, jlong ptr) {
    auto* wrapper = fromJavaPointer<MapWrapper>(ptr);
    auto cameraOptions = wrapper->map->getCameraOptions();
    return maplibre_jni::CameraOptionsConversions::create(env, cameraOptions);
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_MaplibreMap_nativeSetSize
  (JNIEnv* env, jclass, jlong ptr, jobject size) {
    auto* wrapper = fromJavaPointer<MapWrapper>(ptr);
    mbgl::Size mbglSize = maplibre_jni::SizeConversions::extract(env, size);
    wrapper->map->setSize(mbglSize);
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