#pragma once

#include <jni.h>
#include <mbgl/util/geo.hpp>

namespace maplibre_jni {

class LatLngConversions {
public:
    static void init(JNIEnv* env);
    static void destroy(JNIEnv* env);
    
    // Extract mbgl::LatLng from Java LatLng object
    static mbgl::LatLng extract(JNIEnv* env, jobject latLng);
    
    // Create Java LatLng object from mbgl::LatLng
    static jobject create(JNIEnv* env, const mbgl::LatLng& latLng);
    
private:
    static jclass latLngClass;
    static jfieldID latitudeField;
    static jfieldID longitudeField;
    static jmethodID constructor;
    static bool initialized;
};

} // namespace maplibre_jni