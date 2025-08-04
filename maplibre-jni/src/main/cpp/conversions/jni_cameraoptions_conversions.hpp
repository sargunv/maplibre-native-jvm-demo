#pragma once

#include <jni.h>
#include <mbgl/map/camera.hpp>

namespace maplibre_jni {

class CameraOptionsConversions {
public:
    static void init(JNIEnv* env);
    static void destroy(JNIEnv* env);
    
    // Extract mbgl::CameraOptions from Java CameraOptions object
    static mbgl::CameraOptions extract(JNIEnv* env, jobject cameraOptions);
    
    // Create Java CameraOptions object from mbgl::CameraOptions
    static jobject create(JNIEnv* env, const mbgl::CameraOptions& cameraOptions);
    
private:
    static jclass cameraOptionsClass;
    static jfieldID centerField;
    static jfieldID paddingField;
    static jfieldID anchorField;
    static jfieldID zoomField;
    static jfieldID bearingField;
    static jfieldID pitchField;
    static jmethodID constructor;
    static bool initialized;
};

} // namespace maplibre_jni