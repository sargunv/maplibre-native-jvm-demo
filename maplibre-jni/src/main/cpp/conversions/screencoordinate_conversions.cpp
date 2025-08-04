#include "screencoordinate_conversions.hpp"
#include <stdexcept>

namespace maplibre_jni {

// Static member definitions
jclass ScreenCoordinateConversions::screenCoordinateClass = nullptr;
jfieldID ScreenCoordinateConversions::xField = nullptr;
jfieldID ScreenCoordinateConversions::yField = nullptr;
jmethodID ScreenCoordinateConversions::constructor = nullptr;
bool ScreenCoordinateConversions::initialized = false;

void ScreenCoordinateConversions::init(JNIEnv* env) {
    if (initialized) return;
    
    // Find the ScreenCoordinate class
    jclass localClass = env->FindClass("com/maplibre/jni/ScreenCoordinate");
    if (!localClass) {
        throw std::runtime_error("Could not find ScreenCoordinate class");
    }
    
    // Create global reference
    screenCoordinateClass = (jclass)env->NewGlobalRef(localClass);
    env->DeleteLocalRef(localClass);
    
    // Cache field IDs
    xField = env->GetFieldID(screenCoordinateClass, "x", "D");
    if (!xField) {
        throw std::runtime_error("Could not find x field");
    }
    
    yField = env->GetFieldID(screenCoordinateClass, "y", "D");
    if (!yField) {
        throw std::runtime_error("Could not find y field");
    }
    
    // Cache constructor
    constructor = env->GetMethodID(screenCoordinateClass, "<init>", "(DD)V");
    if (!constructor) {
        throw std::runtime_error("Could not find ScreenCoordinate constructor");
    }
    
    initialized = true;
}

void ScreenCoordinateConversions::destroy(JNIEnv* env) {
    if (!initialized) return;
    
    if (screenCoordinateClass) {
        env->DeleteGlobalRef(screenCoordinateClass);
        screenCoordinateClass = nullptr;
    }
    
    xField = nullptr;
    yField = nullptr;
    constructor = nullptr;
    initialized = false;
}

mbgl::ScreenCoordinate ScreenCoordinateConversions::extract(JNIEnv* env, jobject screenCoordinate) {
    if (!initialized) {
        init(env);
    }
    
    if (!screenCoordinate) {
        throw std::invalid_argument("ScreenCoordinate object is null");
    }
    
    double x = env->GetDoubleField(screenCoordinate, xField);
    double y = env->GetDoubleField(screenCoordinate, yField);
    
    return mbgl::ScreenCoordinate{x, y};
}

jobject ScreenCoordinateConversions::create(JNIEnv* env, const mbgl::ScreenCoordinate& screenCoordinate) {
    if (!initialized) {
        init(env);
    }
    
    return env->NewObject(screenCoordinateClass, constructor, 
                          screenCoordinate.x, 
                          screenCoordinate.y);
}

} // namespace maplibre_jni