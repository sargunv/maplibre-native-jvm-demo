#pragma once

#include <jni.h>
#include <mbgl/util/geo.hpp>

namespace maplibre_jni {

class EdgeInsetsConversions {
public:
    static void init(JNIEnv* env);
    static void destroy(JNIEnv* env);
    
    // Extract mbgl::EdgeInsets from Java EdgeInsets object
    static mbgl::EdgeInsets extract(JNIEnv* env, jobject edgeInsets);
    
    // Create Java EdgeInsets object from mbgl::EdgeInsets
    static jobject create(JNIEnv* env, const mbgl::EdgeInsets& edgeInsets);
    
private:
    static jclass edgeInsetsClass;
    static jfieldID topField;
    static jfieldID leftField;
    static jfieldID bottomField;
    static jfieldID rightField;
    static jmethodID constructor;
    static bool initialized;
};

} // namespace maplibre_jni