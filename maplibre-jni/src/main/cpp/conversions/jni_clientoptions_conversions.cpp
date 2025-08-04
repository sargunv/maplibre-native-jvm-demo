#include "jni_clientoptions_conversions.hpp"
#include <stdexcept>

namespace maplibre_jni {

// Static member definitions
jclass ClientOptionsConversions::clientOptionsClass = nullptr;
jfieldID ClientOptionsConversions::nameField = nullptr;
jfieldID ClientOptionsConversions::versionField = nullptr;
jmethodID ClientOptionsConversions::constructor = nullptr;
bool ClientOptionsConversions::initialized = false;

void ClientOptionsConversions::init(JNIEnv* env) {
    if (initialized) return;
    
    // Find the ClientOptions class
    jclass localClass = env->FindClass("com/maplibre/jni/ClientOptions");
    if (!localClass) {
        throw std::runtime_error("Could not find ClientOptions class");
    }
    
    // Create global reference
    clientOptionsClass = (jclass)env->NewGlobalRef(localClass);
    env->DeleteLocalRef(localClass);
    
    // Cache field IDs
    nameField = env->GetFieldID(clientOptionsClass, "name", "Ljava/lang/String;");
    if (!nameField) {
        throw std::runtime_error("Could not find name field");
    }
    
    versionField = env->GetFieldID(clientOptionsClass, "version", "Ljava/lang/String;");
    if (!versionField) {
        throw std::runtime_error("Could not find version field");
    }
    
    // Cache constructor
    constructor = env->GetMethodID(clientOptionsClass, "<init>", 
        "(Ljava/lang/String;Ljava/lang/String;)V");
    if (!constructor) {
        throw std::runtime_error("Could not find ClientOptions constructor");
    }
    
    initialized = true;
}

void ClientOptionsConversions::destroy(JNIEnv* env) {
    if (!initialized) return;
    
    if (clientOptionsClass) {
        env->DeleteGlobalRef(clientOptionsClass);
        clientOptionsClass = nullptr;
    }
    
    nameField = nullptr;
    versionField = nullptr;
    constructor = nullptr;
    initialized = false;
}

mbgl::ClientOptions ClientOptionsConversions::extract(JNIEnv* env, jobject clientOptions) {
    if (!initialized) {
        init(env);
    }
    
    if (!clientOptions) {
        return mbgl::ClientOptions();
    }
    
    mbgl::ClientOptions options;
    
    // Extract name string
    jstring nameStr = (jstring)env->GetObjectField(clientOptions, nameField);
    if (nameStr) {
        const char* name = env->GetStringUTFChars(nameStr, nullptr);
        options.withName(std::string(name));
        env->ReleaseStringUTFChars(nameStr, name);
        env->DeleteLocalRef(nameStr);
    }
    
    // Extract version string
    jstring versionStr = (jstring)env->GetObjectField(clientOptions, versionField);
    if (versionStr) {
        const char* version = env->GetStringUTFChars(versionStr, nullptr);
        options.withVersion(std::string(version));
        env->ReleaseStringUTFChars(versionStr, version);
        env->DeleteLocalRef(versionStr);
    }
    
    return options;
}

jobject ClientOptionsConversions::create(JNIEnv* env, const mbgl::ClientOptions& clientOptions) {
    if (!initialized) {
        init(env);
    }
    
    // Create Java strings
    jstring name = env->NewStringUTF(clientOptions.name().c_str());
    jstring version = env->NewStringUTF(clientOptions.version().c_str());
    
    // Create ClientOptions object
    jobject result = env->NewObject(clientOptionsClass, constructor, name, version);
    
    // Clean up local references
    env->DeleteLocalRef(name);
    env->DeleteLocalRef(version);
    
    return result;
}

} // namespace maplibre_jni