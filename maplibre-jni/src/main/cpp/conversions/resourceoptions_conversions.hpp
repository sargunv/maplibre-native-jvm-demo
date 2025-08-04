#pragma once

#include <jni.h>
#include <mbgl/storage/resource_options.hpp>

namespace maplibre_jni {

class ResourceOptionsConversions {
public:
    static void init(JNIEnv* env);
    static void destroy(JNIEnv* env);
    
    // Extract mbgl::ResourceOptions from Java ResourceOptions object
    static mbgl::ResourceOptions extract(JNIEnv* env, jobject resourceOptions);
    
    // Create Java ResourceOptions object from mbgl::ResourceOptions
    static jobject create(JNIEnv* env, const mbgl::ResourceOptions& resourceOptions);
    
private:
    static jclass resourceOptionsClass;
    static jfieldID apiKeyField;
    static jfieldID tileServerOptionsField;
    static jfieldID cachePathField;
    static jfieldID assetPathField;
    static jfieldID maximumCacheSizeField;
    static jmethodID constructor;
    static bool initialized;
};

} // namespace maplibre_jni