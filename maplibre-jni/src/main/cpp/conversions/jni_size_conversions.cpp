#include "jni_size_conversions.hpp"
#include <stdexcept>

namespace maplibre_jni {

// Static member definitions
jclass SizeConversions::sizeClass = nullptr;
jfieldID SizeConversions::widthField = nullptr;
jfieldID SizeConversions::heightField = nullptr;
jmethodID SizeConversions::constructor = nullptr;
bool SizeConversions::initialized = false;

void SizeConversions::init(JNIEnv* env) {
    if (initialized) return;
    
    // Find the Size class
    jclass localClass = env->FindClass("com/maplibre/jni/Size");
    if (!localClass) {
        throw std::runtime_error("Could not find Size class");
    }
    
    // Create global reference
    sizeClass = (jclass)env->NewGlobalRef(localClass);
    env->DeleteLocalRef(localClass);
    
    // Cache field IDs
    widthField = env->GetFieldID(sizeClass, "width", "I");
    if (!widthField) {
        throw std::runtime_error("Could not find width field");
    }
    
    heightField = env->GetFieldID(sizeClass, "height", "I");
    if (!heightField) {
        throw std::runtime_error("Could not find height field");
    }
    
    // Cache constructor
    constructor = env->GetMethodID(sizeClass, "<init>", "(II)V");
    if (!constructor) {
        throw std::runtime_error("Could not find Size constructor");
    }
    
    initialized = true;
}

void SizeConversions::destroy(JNIEnv* env) {
    if (!initialized) return;
    
    if (sizeClass) {
        env->DeleteGlobalRef(sizeClass);
        sizeClass = nullptr;
    }
    
    widthField = nullptr;
    heightField = nullptr;
    constructor = nullptr;
    initialized = false;
}

mbgl::Size SizeConversions::extract(JNIEnv* env, jobject size) {
    if (!initialized) {
        init(env);
    }
    
    if (!size) {
        throw std::invalid_argument("Size object is null");
    }
    
    jint width = env->GetIntField(size, widthField);
    jint height = env->GetIntField(size, heightField);
    
    return mbgl::Size(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
}

jobject SizeConversions::create(JNIEnv* env, const mbgl::Size& size) {
    if (!initialized) {
        init(env);
    }
    
    return env->NewObject(sizeClass, constructor, 
                          static_cast<jint>(size.width), 
                          static_cast<jint>(size.height));
}

} // namespace maplibre_jni