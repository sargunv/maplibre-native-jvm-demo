#pragma once

#include <jni.h>
#include <mbgl/util/size.hpp>

namespace maplibre_jni {

class SizeConversions {
public:
    static void init(JNIEnv* env);
    static void destroy(JNIEnv* env);
    
    // Extract mbgl::Size from Java Size object
    static mbgl::Size extract(JNIEnv* env, jobject size);
    
    // Create Java Size object from mbgl::Size
    static jobject create(JNIEnv* env, const mbgl::Size& size);
    
private:
    static jclass sizeClass;
    static jfieldID widthField;
    static jfieldID heightField;
    static jmethodID constructor;
    static bool initialized;
};

} // namespace maplibre_jni