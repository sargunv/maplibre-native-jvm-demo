#pragma once

#include <jni.h>
#include <mbgl/util/client_options.hpp>

namespace maplibre_jni {

class ClientOptionsConversions {
public:
    static void init(JNIEnv* env);
    static void destroy(JNIEnv* env);
    
    // Extract mbgl::ClientOptions from Java ClientOptions object
    static mbgl::ClientOptions extract(JNIEnv* env, jobject clientOptions);
    
    // Create Java ClientOptions object from mbgl::ClientOptions
    static jobject create(JNIEnv* env, const mbgl::ClientOptions& clientOptions);
    
private:
    static jclass clientOptionsClass;
    static jfieldID nameField;
    static jfieldID versionField;
    static jmethodID constructor;
    static bool initialized;
};

} // namespace maplibre_jni