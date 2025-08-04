#include "cameraoptions_conversions.hpp"
#include "latlng_conversions.hpp"
#include "edgeinsets_conversions.hpp"
#include "screencoordinate_conversions.hpp"
#include <stdexcept>

namespace maplibre_jni {

// Static member definitions
jclass CameraOptionsConversions::cameraOptionsClass = nullptr;
jfieldID CameraOptionsConversions::centerField = nullptr;
jfieldID CameraOptionsConversions::paddingField = nullptr;
jfieldID CameraOptionsConversions::anchorField = nullptr;
jfieldID CameraOptionsConversions::zoomField = nullptr;
jfieldID CameraOptionsConversions::bearingField = nullptr;
jfieldID CameraOptionsConversions::pitchField = nullptr;
jmethodID CameraOptionsConversions::constructor = nullptr;
bool CameraOptionsConversions::initialized = false;

void CameraOptionsConversions::init(JNIEnv* env) {
    if (initialized) return;
    
    // Find the CameraOptions class
    jclass localClass = env->FindClass("com/maplibre/jni/CameraOptions");
    if (!localClass) {
        throw std::runtime_error("Could not find CameraOptions class");
    }
    
    // Create global reference
    cameraOptionsClass = (jclass)env->NewGlobalRef(localClass);
    env->DeleteLocalRef(localClass);
    
    // Cache field IDs
    centerField = env->GetFieldID(cameraOptionsClass, "center", "Lcom/maplibre/jni/LatLng;");
    if (!centerField) {
        throw std::runtime_error("Could not find center field");
    }
    
    paddingField = env->GetFieldID(cameraOptionsClass, "padding", "Lcom/maplibre/jni/EdgeInsets;");
    if (!paddingField) {
        throw std::runtime_error("Could not find padding field");
    }
    
    anchorField = env->GetFieldID(cameraOptionsClass, "anchor", "Lcom/maplibre/jni/ScreenCoordinate;");
    if (!anchorField) {
        throw std::runtime_error("Could not find anchor field");
    }
    
    zoomField = env->GetFieldID(cameraOptionsClass, "zoom", "Ljava/lang/Double;");
    if (!zoomField) {
        throw std::runtime_error("Could not find zoom field");
    }
    
    bearingField = env->GetFieldID(cameraOptionsClass, "bearing", "Ljava/lang/Double;");
    if (!bearingField) {
        throw std::runtime_error("Could not find bearing field");
    }
    
    pitchField = env->GetFieldID(cameraOptionsClass, "pitch", "Ljava/lang/Double;");
    if (!pitchField) {
        throw std::runtime_error("Could not find pitch field");
    }
    
    // Cache constructor
    constructor = env->GetMethodID(cameraOptionsClass, "<init>", 
        "(Lcom/maplibre/jni/LatLng;Lcom/maplibre/jni/EdgeInsets;Lcom/maplibre/jni/ScreenCoordinate;Ljava/lang/Double;Ljava/lang/Double;Ljava/lang/Double;)V");
    if (!constructor) {
        throw std::runtime_error("Could not find CameraOptions constructor");
    }
    
    // Initialize dependent conversions
    LatLngConversions::init(env);
    EdgeInsetsConversions::init(env);
    ScreenCoordinateConversions::init(env);
    
    initialized = true;
}

void CameraOptionsConversions::destroy(JNIEnv* env) {
    if (!initialized) return;
    
    if (cameraOptionsClass) {
        env->DeleteGlobalRef(cameraOptionsClass);
        cameraOptionsClass = nullptr;
    }
    
    centerField = nullptr;
    paddingField = nullptr;
    anchorField = nullptr;
    zoomField = nullptr;
    bearingField = nullptr;
    pitchField = nullptr;
    constructor = nullptr;
    initialized = false;
}

mbgl::CameraOptions CameraOptionsConversions::extract(JNIEnv* env, jobject cameraOptions) {
    if (!initialized) {
        init(env);
    }
    
    if (!cameraOptions) {
        return mbgl::CameraOptions();
    }
    
    mbgl::CameraOptions options;
    
    // Extract center
    jobject centerObj = env->GetObjectField(cameraOptions, centerField);
    if (centerObj) {
        options.center = LatLngConversions::extract(env, centerObj);
        env->DeleteLocalRef(centerObj);
    }
    
    // Extract padding
    jobject paddingObj = env->GetObjectField(cameraOptions, paddingField);
    if (paddingObj) {
        options.padding = EdgeInsetsConversions::extract(env, paddingObj);
        env->DeleteLocalRef(paddingObj);
    }
    
    // Extract anchor
    jobject anchorObj = env->GetObjectField(cameraOptions, anchorField);
    if (anchorObj) {
        options.anchor = ScreenCoordinateConversions::extract(env, anchorObj);
        env->DeleteLocalRef(anchorObj);
    }
    
    // Extract zoom (Double object)
    jobject zoomObj = env->GetObjectField(cameraOptions, zoomField);
    if (zoomObj) {
        jclass doubleClass = env->FindClass("java/lang/Double");
        jmethodID doubleValue = env->GetMethodID(doubleClass, "doubleValue", "()D");
        options.zoom = env->CallDoubleMethod(zoomObj, doubleValue);
        env->DeleteLocalRef(zoomObj);
        env->DeleteLocalRef(doubleClass);
    }
    
    // Extract bearing (Double object)
    jobject bearingObj = env->GetObjectField(cameraOptions, bearingField);
    if (bearingObj) {
        jclass doubleClass = env->FindClass("java/lang/Double");
        jmethodID doubleValue = env->GetMethodID(doubleClass, "doubleValue", "()D");
        options.bearing = env->CallDoubleMethod(bearingObj, doubleValue);
        env->DeleteLocalRef(bearingObj);
        env->DeleteLocalRef(doubleClass);
    }
    
    // Extract pitch (Double object)
    jobject pitchObj = env->GetObjectField(cameraOptions, pitchField);
    if (pitchObj) {
        jclass doubleClass = env->FindClass("java/lang/Double");
        jmethodID doubleValue = env->GetMethodID(doubleClass, "doubleValue", "()D");
        options.pitch = env->CallDoubleMethod(pitchObj, doubleValue);
        env->DeleteLocalRef(pitchObj);
        env->DeleteLocalRef(doubleClass);
    }
    
    return options;
}

jobject CameraOptionsConversions::create(JNIEnv* env, const mbgl::CameraOptions& cameraOptions) {
    if (!initialized) {
        init(env);
    }
    
    // Create nullable field values
    jobject center = nullptr;
    if (cameraOptions.center.has_value()) {
        center = LatLngConversions::create(env, cameraOptions.center.value());
    }
    
    jobject padding = nullptr;
    if (cameraOptions.padding.has_value()) {
        padding = EdgeInsetsConversions::create(env, cameraOptions.padding.value());
    }
    
    jobject anchor = nullptr;
    if (cameraOptions.anchor.has_value()) {
        anchor = ScreenCoordinateConversions::create(env, cameraOptions.anchor.value());
    }
    
    // Create Double objects for nullable numeric fields
    jclass doubleClass = env->FindClass("java/lang/Double");
    jmethodID valueOf = env->GetStaticMethodID(doubleClass, "valueOf", "(D)Ljava/lang/Double;");
    
    jobject zoom = nullptr;
    if (cameraOptions.zoom.has_value()) {
        zoom = env->CallStaticObjectMethod(doubleClass, valueOf, cameraOptions.zoom.value());
    }
    
    jobject bearing = nullptr;
    if (cameraOptions.bearing.has_value()) {
        bearing = env->CallStaticObjectMethod(doubleClass, valueOf, cameraOptions.bearing.value());
    }
    
    jobject pitch = nullptr;
    if (cameraOptions.pitch.has_value()) {
        pitch = env->CallStaticObjectMethod(doubleClass, valueOf, cameraOptions.pitch.value());
    }
    
    env->DeleteLocalRef(doubleClass);
    
    // Create CameraOptions object
    jobject result = env->NewObject(cameraOptionsClass, constructor, 
                                    center, padding, anchor, zoom, bearing, pitch);
    
    // Clean up local references
    if (center) env->DeleteLocalRef(center);
    if (padding) env->DeleteLocalRef(padding);
    if (anchor) env->DeleteLocalRef(anchor);
    if (zoom) env->DeleteLocalRef(zoom);
    if (bearing) env->DeleteLocalRef(bearing);
    if (pitch) env->DeleteLocalRef(pitch);
    
    return result;
}

} // namespace maplibre_jni