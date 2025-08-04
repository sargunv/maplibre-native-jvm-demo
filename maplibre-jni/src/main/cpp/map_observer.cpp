#include "map_observer.hpp"
#include <cassert>
#include <iostream>

// Simple logging for desktop
#define LOGE(...) do { \
    fprintf(stderr, "MapLibreJNI ERROR: "); \
    fprintf(stderr, __VA_ARGS__); \
    fprintf(stderr, "\n"); \
} while(0)

namespace maplibre_jni {

JniMapObserver::JniMapObserver(JNIEnv* env, jobject kotlinObserver) {
    // Get JavaVM
    if (env->GetJavaVM(&jvm) != JNI_OK) {
        throw std::runtime_error("Failed to get JavaVM");
    }
    
    // Create global reference to the observer
    observer = env->NewGlobalRef(kotlinObserver);
    if (!observer) {
        throw std::runtime_error("Failed to create global reference");
    }
    
    // Get MapObserver class and cache method IDs
    jclass observerClass = env->GetObjectClass(kotlinObserver);
    
    onCameraWillChangeMethod = env->GetMethodID(observerClass, "onCameraWillChange", "(Lcom/maplibre/jni/MapObserver$CameraChangeMode;)V");
    onCameraIsChangingMethod = env->GetMethodID(observerClass, "onCameraIsChanging", "()V");
    onCameraDidChangeMethod = env->GetMethodID(observerClass, "onCameraDidChange", "(Lcom/maplibre/jni/MapObserver$CameraChangeMode;)V");
    onWillStartLoadingMapMethod = env->GetMethodID(observerClass, "onWillStartLoadingMap", "()V");
    onDidFinishLoadingMapMethod = env->GetMethodID(observerClass, "onDidFinishLoadingMap", "()V");
    onDidFailLoadingMapMethod = env->GetMethodID(observerClass, "onDidFailLoadingMap", "(Lcom/maplibre/jni/MapLoadError;Ljava/lang/String;)V");
    onWillStartRenderingFrameMethod = env->GetMethodID(observerClass, "onWillStartRenderingFrame", "()V");
    onDidFinishRenderingFrameMethod = env->GetMethodID(observerClass, "onDidFinishRenderingFrame", "(Lcom/maplibre/jni/MapObserver$RenderFrameStatus;)V");
    onWillStartRenderingMapMethod = env->GetMethodID(observerClass, "onWillStartRenderingMap", "()V");
    onDidFinishRenderingMapMethod = env->GetMethodID(observerClass, "onDidFinishRenderingMap", "(Lcom/maplibre/jni/MapObserver$RenderMode;)V");
    onDidFinishLoadingStyleMethod = env->GetMethodID(observerClass, "onDidFinishLoadingStyle", "()V");
    onStyleImageMissingMethod = env->GetMethodID(observerClass, "onStyleImageMissing", "(Ljava/lang/String;)V");
    onDidBecomeIdleMethod = env->GetMethodID(observerClass, "onDidBecomeIdle", "()V");
    
    // Cache enum classes
    cameraChangeModeClass = (jclass)env->NewGlobalRef(env->FindClass("com/maplibre/jni/MapObserver$CameraChangeMode"));
    renderModeClass = (jclass)env->NewGlobalRef(env->FindClass("com/maplibre/jni/MapObserver$RenderMode"));
    renderFrameStatusClass = (jclass)env->NewGlobalRef(env->FindClass("com/maplibre/jni/MapObserver$RenderFrameStatus"));
    mapLoadErrorClass = (jclass)env->NewGlobalRef(env->FindClass("com/maplibre/jni/MapLoadError"));
    
    env->DeleteLocalRef(observerClass);
}

JniMapObserver::~JniMapObserver() {
    JNIEnv* env = getEnv();
    if (env) {
        env->DeleteGlobalRef(observer);
        env->DeleteGlobalRef(cameraChangeModeClass);
        env->DeleteGlobalRef(renderModeClass);
        env->DeleteGlobalRef(renderFrameStatusClass);
        env->DeleteGlobalRef(mapLoadErrorClass);
    }
}

JNIEnv* JniMapObserver::getEnv() {
    JNIEnv* env = nullptr;
    jint result = jvm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6);
    
    if (result == JNI_EDETACHED) {
        // Attach current thread
        if (jvm->AttachCurrentThread(reinterpret_cast<void**>(&env), nullptr) != JNI_OK) {
            LOGE("Failed to attach current thread");
            return nullptr;
        }
    } else if (result != JNI_OK) {
        LOGE("Failed to get JNIEnv");
        return nullptr;
    }
    
    return env;
}

jobject JniMapObserver::convertCameraChangeMode(JNIEnv* env, mbgl::MapObserver::CameraChangeMode mode) {
    jfieldID fieldId;
    switch (mode) {
        case mbgl::MapObserver::CameraChangeMode::Immediate:
            fieldId = env->GetStaticFieldID(cameraChangeModeClass, "IMMEDIATE", "Lcom/maplibre/jni/MapObserver$CameraChangeMode;");
            break;
        case mbgl::MapObserver::CameraChangeMode::Animated:
            fieldId = env->GetStaticFieldID(cameraChangeModeClass, "ANIMATED", "Lcom/maplibre/jni/MapObserver$CameraChangeMode;");
            break;
        default:
            return nullptr;
    }
    return env->GetStaticObjectField(cameraChangeModeClass, fieldId);
}

jobject JniMapObserver::convertRenderMode(JNIEnv* env, mbgl::MapObserver::RenderMode mode) {
    jfieldID fieldId;
    switch (mode) {
        case mbgl::MapObserver::RenderMode::Partial:
            fieldId = env->GetStaticFieldID(renderModeClass, "PARTIAL", "Lcom/maplibre/jni/MapObserver$RenderMode;");
            break;
        case mbgl::MapObserver::RenderMode::Full:
            fieldId = env->GetStaticFieldID(renderModeClass, "FULL", "Lcom/maplibre/jni/MapObserver$RenderMode;");
            break;
        default:
            return nullptr;
    }
    return env->GetStaticObjectField(renderModeClass, fieldId);
}

jobject JniMapObserver::convertMapLoadError(JNIEnv* env, mbgl::MapLoadError error) {
    jfieldID fieldId;
    switch (error) {
        case mbgl::MapLoadError::StyleParseError:
            fieldId = env->GetStaticFieldID(mapLoadErrorClass, "STYLE_PARSE_ERROR", "Lcom/maplibre/jni/MapLoadError;");
            break;
        case mbgl::MapLoadError::StyleLoadError:
            fieldId = env->GetStaticFieldID(mapLoadErrorClass, "STYLE_LOAD_ERROR", "Lcom/maplibre/jni/MapLoadError;");
            break;
        case mbgl::MapLoadError::NotFoundError:
            fieldId = env->GetStaticFieldID(mapLoadErrorClass, "NOT_FOUND_ERROR", "Lcom/maplibre/jni/MapLoadError;");
            break;
        case mbgl::MapLoadError::UnknownError:
        default:
            fieldId = env->GetStaticFieldID(mapLoadErrorClass, "UNKNOWN_ERROR", "Lcom/maplibre/jni/MapLoadError;");
            break;
    }
    return env->GetStaticObjectField(mapLoadErrorClass, fieldId);
}

jobject JniMapObserver::createRenderFrameStatus(JNIEnv* env, const mbgl::MapObserver::RenderFrameStatus& status) {
    jmethodID constructor = env->GetMethodID(renderFrameStatusClass, "<init>", 
        "(Lcom/maplibre/jni/MapObserver$RenderMode;ZZ)V");
    
    jobject renderMode = convertRenderMode(env, status.mode);
    jobject result = env->NewObject(renderFrameStatusClass, constructor, 
        renderMode, status.needsRepaint, status.placementChanged);
    
    env->DeleteLocalRef(renderMode);
    return result;
}

// Camera events
void JniMapObserver::onCameraWillChange(mbgl::MapObserver::CameraChangeMode mode) {
    JNIEnv* env = getEnv();
    if (!env) return;
    
    jobject jmode = convertCameraChangeMode(env, mode);
    env->CallVoidMethod(observer, onCameraWillChangeMethod, jmode);
    env->DeleteLocalRef(jmode);
}

void JniMapObserver::onCameraIsChanging() {
    JNIEnv* env = getEnv();
    if (!env) return;
    
    env->CallVoidMethod(observer, onCameraIsChangingMethod);
}

void JniMapObserver::onCameraDidChange(mbgl::MapObserver::CameraChangeMode mode) {
    JNIEnv* env = getEnv();
    if (!env) return;
    
    jobject jmode = convertCameraChangeMode(env, mode);
    env->CallVoidMethod(observer, onCameraDidChangeMethod, jmode);
    env->DeleteLocalRef(jmode);
}

// Map loading events
void JniMapObserver::onWillStartLoadingMap() {
    JNIEnv* env = getEnv();
    if (!env) return;
    
    env->CallVoidMethod(observer, onWillStartLoadingMapMethod);
}

void JniMapObserver::onDidFinishLoadingMap() {
    JNIEnv* env = getEnv();
    if (!env) return;
    
    env->CallVoidMethod(observer, onDidFinishLoadingMapMethod);
}

void JniMapObserver::onDidFailLoadingMap(mbgl::MapLoadError error, const std::string& message) {
    JNIEnv* env = getEnv();
    if (!env) return;
    
    jobject jerror = convertMapLoadError(env, error);
    jstring jmessage = env->NewStringUTF(message.c_str());
    
    env->CallVoidMethod(observer, onDidFailLoadingMapMethod, jerror, jmessage);
    
    env->DeleteLocalRef(jerror);
    env->DeleteLocalRef(jmessage);
}

// Rendering events
void JniMapObserver::onWillStartRenderingFrame() {
    JNIEnv* env = getEnv();
    if (!env) return;
    
    env->CallVoidMethod(observer, onWillStartRenderingFrameMethod);
}

void JniMapObserver::onDidFinishRenderingFrame(const mbgl::MapObserver::RenderFrameStatus& status) {
    JNIEnv* env = getEnv();
    if (!env) return;
    
    jobject jstatus = createRenderFrameStatus(env, status);
    env->CallVoidMethod(observer, onDidFinishRenderingFrameMethod, jstatus);
    env->DeleteLocalRef(jstatus);
}

void JniMapObserver::onWillStartRenderingMap() {
    JNIEnv* env = getEnv();
    if (!env) return;
    
    env->CallVoidMethod(observer, onWillStartRenderingMapMethod);
}

void JniMapObserver::onDidFinishRenderingMap(mbgl::MapObserver::RenderMode mode) {
    JNIEnv* env = getEnv();
    if (!env) return;
    
    jobject jmode = convertRenderMode(env, mode);
    env->CallVoidMethod(observer, onDidFinishRenderingMapMethod, jmode);
    env->DeleteLocalRef(jmode);
}

// Style events
void JniMapObserver::onDidFinishLoadingStyle() {
    JNIEnv* env = getEnv();
    if (!env) return;
    
    env->CallVoidMethod(observer, onDidFinishLoadingStyleMethod);
}

void JniMapObserver::onStyleImageMissing(const std::string& imageId) {
    JNIEnv* env = getEnv();
    if (!env) return;
    
    jstring jimageId = env->NewStringUTF(imageId.c_str());
    env->CallVoidMethod(observer, onStyleImageMissingMethod, jimageId);
    env->DeleteLocalRef(jimageId);
}

// Idle state
void JniMapObserver::onDidBecomeIdle() {
    JNIEnv* env = getEnv();
    if (!env) return;
    
    env->CallVoidMethod(observer, onDidBecomeIdleMethod);
}

}