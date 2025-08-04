#pragma once

#include <jni.h>
#include <mbgl/map/map_options.hpp>

namespace maplibre_jni {

class MapOptionsConversions {
public:
    static void init(JNIEnv* env);
    static void destroy(JNIEnv* env);
    
    // Extract mbgl::MapOptions from Java MapOptions object
    static mbgl::MapOptions extract(JNIEnv* env, jobject mapOptions);
    
    // Create Java MapOptions object from mbgl::MapOptions
    static jobject create(JNIEnv* env, const mbgl::MapOptions& mapOptions);
    
private:
    static jclass mapOptionsClass;
    static jfieldID mapModeField;
    static jfieldID constrainModeField;
    static jfieldID viewportModeField;
    static jfieldID crossSourceCollisionsField;
    static jfieldID northOrientationField;
    static jfieldID sizeField;
    static jfieldID pixelRatioField;
    static jmethodID constructor;
    static bool initialized;
};

} // namespace maplibre_jni