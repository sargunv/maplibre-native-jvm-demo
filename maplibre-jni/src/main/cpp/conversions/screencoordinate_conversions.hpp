#pragma once

#include <jni.h>
#include <mbgl/util/geo.hpp>

namespace maplibre_jni {

class ScreenCoordinateConversions {
public:
    static void init(JNIEnv* env);
    static void destroy(JNIEnv* env);
    
    // Extract mbgl::ScreenCoordinate from Java ScreenCoordinate object
    static mbgl::ScreenCoordinate extract(JNIEnv* env, jobject screenCoordinate);
    
    // Create Java ScreenCoordinate object from mbgl::ScreenCoordinate
    static jobject create(JNIEnv* env, const mbgl::ScreenCoordinate& screenCoordinate);
    
private:
    static jclass screenCoordinateClass;
    static jfieldID xField;
    static jfieldID yField;
    static jmethodID constructor;
    static bool initialized;
};

} // namespace maplibre_jni