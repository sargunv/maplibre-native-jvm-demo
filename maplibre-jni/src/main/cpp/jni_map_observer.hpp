#pragma once

#include <mbgl/map/map_observer.hpp>
#include <jni.h>
#include <memory>

namespace maplibre_jni {

// C++ implementation that forwards MapObserver callbacks to a Kotlin MapObserver
class JniMapObserver : public mbgl::MapObserver {
public:
    JniMapObserver(JNIEnv* env, jobject kotlinObserver);
    ~JniMapObserver() override;

    // Camera events
    void onCameraWillChange(mbgl::MapObserver::CameraChangeMode mode) override;
    void onCameraIsChanging() override;
    void onCameraDidChange(mbgl::MapObserver::CameraChangeMode mode) override;
    
    // Map loading events
    void onWillStartLoadingMap() override;
    void onDidFinishLoadingMap() override;
    void onDidFailLoadingMap(mbgl::MapLoadError error, const std::string& message) override;
    
    // Rendering events
    void onWillStartRenderingFrame() override;
    void onDidFinishRenderingFrame(const mbgl::MapObserver::RenderFrameStatus& status) override;
    void onWillStartRenderingMap() override;
    void onDidFinishRenderingMap(mbgl::MapObserver::RenderMode mode) override;
    
    // Style events
    void onDidFinishLoadingStyle() override;
    void onStyleImageMissing(const std::string& imageId) override;
    
    // Idle state
    void onDidBecomeIdle() override;
    
    // TODO: Implement these when we have the necessary type wrappers:
    // - onSourceChanged
    // - onCanRemoveUnusedStyleImage
    // - onRegisterShaders
    // - Shader compilation callbacks
    // - Glyph/Tile/Sprite loading callbacks

private:
    JavaVM* jvm;
    jobject observer; // Global reference to Kotlin MapObserver
    
    // Cached method IDs for better performance
    jmethodID onCameraWillChangeMethod;
    jmethodID onCameraIsChangingMethod;
    jmethodID onCameraDidChangeMethod;
    jmethodID onWillStartLoadingMapMethod;
    jmethodID onDidFinishLoadingMapMethod;
    jmethodID onDidFailLoadingMapMethod;
    jmethodID onWillStartRenderingFrameMethod;
    jmethodID onDidFinishRenderingFrameMethod;
    jmethodID onWillStartRenderingMapMethod;
    jmethodID onDidFinishRenderingMapMethod;
    jmethodID onDidFinishLoadingStyleMethod;
    jmethodID onStyleImageMissingMethod;
    jmethodID onDidBecomeIdleMethod;
    
    // Cached class references
    jclass cameraChangeModeClass;
    jclass renderModeClass;
    jclass renderFrameStatusClass;
    jclass mapLoadErrorClass;
    
    // Helper to get JNIEnv for current thread
    JNIEnv* getEnv();
    
    // Helpers to convert enums
    jobject convertCameraChangeMode(JNIEnv* env, mbgl::MapObserver::CameraChangeMode mode);
    jobject convertRenderMode(JNIEnv* env, mbgl::MapObserver::RenderMode mode);
    jobject convertMapLoadError(JNIEnv* env, mbgl::MapLoadError error);
    jobject createRenderFrameStatus(JNIEnv* env, const mbgl::MapObserver::RenderFrameStatus& status);
};

}