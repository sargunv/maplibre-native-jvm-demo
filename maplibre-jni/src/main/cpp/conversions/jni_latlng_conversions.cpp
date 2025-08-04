#include "jni_latlng_conversions.hpp"
#include <stdexcept>

namespace maplibre_jni {

// Static member definitions
jclass LatLngConversions::latLngClass = nullptr;
jfieldID LatLngConversions::latitudeField = nullptr;
jfieldID LatLngConversions::longitudeField = nullptr;
jmethodID LatLngConversions::constructor = nullptr;
bool LatLngConversions::initialized = false;

void LatLngConversions::init(JNIEnv* env) {
    if (initialized) return;
    
    // Find the LatLng class
    jclass localClass = env->FindClass("com/maplibre/jni/LatLng");
    if (!localClass) {
        throw std::runtime_error("Could not find LatLng class");
    }
    
    // Create global reference
    latLngClass = (jclass)env->NewGlobalRef(localClass);
    env->DeleteLocalRef(localClass);
    
    // Cache field IDs
    latitudeField = env->GetFieldID(latLngClass, "latitude", "D");
    if (!latitudeField) {
        throw std::runtime_error("Could not find latitude field");
    }
    
    longitudeField = env->GetFieldID(latLngClass, "longitude", "D");
    if (!longitudeField) {
        throw std::runtime_error("Could not find longitude field");
    }
    
    // Cache constructor
    constructor = env->GetMethodID(latLngClass, "<init>", "(DD)V");
    if (!constructor) {
        throw std::runtime_error("Could not find LatLng constructor");
    }
    
    initialized = true;
}

void LatLngConversions::destroy(JNIEnv* env) {
    if (!initialized) return;
    
    if (latLngClass) {
        env->DeleteGlobalRef(latLngClass);
        latLngClass = nullptr;
    }
    
    latitudeField = nullptr;
    longitudeField = nullptr;
    constructor = nullptr;
    initialized = false;
}

mbgl::LatLng LatLngConversions::extract(JNIEnv* env, jobject latLng) {
    if (!initialized) {
        init(env);
    }
    
    if (!latLng) {
        throw std::invalid_argument("LatLng object is null");
    }
    
    double latitude = env->GetDoubleField(latLng, latitudeField);
    double longitude = env->GetDoubleField(latLng, longitudeField);
    
    return mbgl::LatLng(latitude, longitude);
}

jobject LatLngConversions::create(JNIEnv* env, const mbgl::LatLng& latLng) {
    if (!initialized) {
        init(env);
    }
    
    return env->NewObject(latLngClass, constructor, 
                          latLng.latitude(), 
                          latLng.longitude());
}

} // namespace maplibre_jni