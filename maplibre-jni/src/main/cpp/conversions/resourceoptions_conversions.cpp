#include "resourceoptions_conversions.hpp"
#include "tileserveroptions_conversions.hpp"
#include <stdexcept>

namespace maplibre_jni {

// Static member definitions
jclass ResourceOptionsConversions::resourceOptionsClass = nullptr;
jfieldID ResourceOptionsConversions::apiKeyField = nullptr;
jfieldID ResourceOptionsConversions::tileServerOptionsField = nullptr;
jfieldID ResourceOptionsConversions::cachePathField = nullptr;
jfieldID ResourceOptionsConversions::assetPathField = nullptr;
jfieldID ResourceOptionsConversions::maximumCacheSizeField = nullptr;
jmethodID ResourceOptionsConversions::constructor = nullptr;
bool ResourceOptionsConversions::initialized = false;

void ResourceOptionsConversions::init(JNIEnv* env) {
    if (initialized) return;
    
    // Find the ResourceOptions class
    jclass localClass = env->FindClass("com/maplibre/jni/ResourceOptions");
    if (!localClass) {
        throw std::runtime_error("Could not find ResourceOptions class");
    }
    
    // Create global reference
    resourceOptionsClass = (jclass)env->NewGlobalRef(localClass);
    env->DeleteLocalRef(localClass);
    
    // Cache field IDs
    apiKeyField = env->GetFieldID(resourceOptionsClass, "apiKey", "Ljava/lang/String;");
    if (!apiKeyField) {
        throw std::runtime_error("Could not find apiKey field");
    }
    
    tileServerOptionsField = env->GetFieldID(resourceOptionsClass, "tileServerOptions", "Lcom/maplibre/jni/TileServerOptions;");
    if (!tileServerOptionsField) {
        throw std::runtime_error("Could not find tileServerOptions field");
    }
    
    cachePathField = env->GetFieldID(resourceOptionsClass, "cachePath", "Ljava/lang/String;");
    if (!cachePathField) {
        throw std::runtime_error("Could not find cachePath field");
    }
    
    assetPathField = env->GetFieldID(resourceOptionsClass, "assetPath", "Ljava/lang/String;");
    if (!assetPathField) {
        throw std::runtime_error("Could not find assetPath field");
    }
    
    maximumCacheSizeField = env->GetFieldID(resourceOptionsClass, "maximumCacheSize", "J");
    if (!maximumCacheSizeField) {
        throw std::runtime_error("Could not find maximumCacheSize field");
    }
    
    // Cache constructor
    constructor = env->GetMethodID(resourceOptionsClass, "<init>", 
        "(Ljava/lang/String;Lcom/maplibre/jni/TileServerOptions;Ljava/lang/String;Ljava/lang/String;J)V");
    if (!constructor) {
        throw std::runtime_error("Could not find ResourceOptions constructor");
    }
    
    // Initialize dependent conversions
    TileServerOptionsConversions::init(env);
    
    initialized = true;
}

void ResourceOptionsConversions::destroy(JNIEnv* env) {
    if (!initialized) return;
    
    if (resourceOptionsClass) {
        env->DeleteGlobalRef(resourceOptionsClass);
        resourceOptionsClass = nullptr;
    }
    
    apiKeyField = nullptr;
    tileServerOptionsField = nullptr;
    cachePathField = nullptr;
    assetPathField = nullptr;
    maximumCacheSizeField = nullptr;
    constructor = nullptr;
    initialized = false;
}

mbgl::ResourceOptions ResourceOptionsConversions::extract(JNIEnv* env, jobject resourceOptions) {
    if (!initialized) {
        init(env);
    }
    
    if (!resourceOptions) {
        return mbgl::ResourceOptions::Default();
    }
    
    mbgl::ResourceOptions options;
    
    // Extract apiKey string
    jstring apiKeyStr = (jstring)env->GetObjectField(resourceOptions, apiKeyField);
    if (apiKeyStr) {
        const char* apiKey = env->GetStringUTFChars(apiKeyStr, nullptr);
        options.withApiKey(std::string(apiKey));
        env->ReleaseStringUTFChars(apiKeyStr, apiKey);
        env->DeleteLocalRef(apiKeyStr);
    }
    
    // Extract TileServerOptions
    jobject tileServerOptionsObj = env->GetObjectField(resourceOptions, tileServerOptionsField);
    if (tileServerOptionsObj) {
        mbgl::TileServerOptions tileServerOptions = TileServerOptionsConversions::extract(env, tileServerOptionsObj);
        options.withTileServerOptions(tileServerOptions);
        env->DeleteLocalRef(tileServerOptionsObj);
    }
    
    // Extract cachePath string
    jstring cachePathStr = (jstring)env->GetObjectField(resourceOptions, cachePathField);
    if (cachePathStr) {
        const char* cachePath = env->GetStringUTFChars(cachePathStr, nullptr);
        options.withCachePath(std::string(cachePath));
        env->ReleaseStringUTFChars(cachePathStr, cachePath);
        env->DeleteLocalRef(cachePathStr);
    }
    
    // Extract assetPath string
    jstring assetPathStr = (jstring)env->GetObjectField(resourceOptions, assetPathField);
    if (assetPathStr) {
        const char* assetPath = env->GetStringUTFChars(assetPathStr, nullptr);
        options.withAssetPath(std::string(assetPath));
        env->ReleaseStringUTFChars(assetPathStr, assetPath);
        env->DeleteLocalRef(assetPathStr);
    }
    
    // Extract maximumCacheSize
    jlong maximumCacheSize = env->GetLongField(resourceOptions, maximumCacheSizeField);
    options.withMaximumCacheSize(static_cast<uint64_t>(maximumCacheSize));
    
    // Note: platformContext is not supported for desktop JVM
    // It would be set to nullptr which is appropriate for desktop platforms
    
    return options;
}

jobject ResourceOptionsConversions::create(JNIEnv* env, const mbgl::ResourceOptions& resourceOptions) {
    if (!initialized) {
        init(env);
    }
    
    // Create Java strings
    jstring apiKey = env->NewStringUTF(resourceOptions.apiKey().c_str());
    jstring cachePath = env->NewStringUTF(resourceOptions.cachePath().c_str());
    jstring assetPath = env->NewStringUTF(resourceOptions.assetPath().c_str());
    
    // Create TileServerOptions object
    jobject tileServerOptions = TileServerOptionsConversions::create(env, resourceOptions.tileServerOptions());
    
    // Create ResourceOptions object
    jobject result = env->NewObject(resourceOptionsClass, constructor,
                                    apiKey, tileServerOptions, cachePath, assetPath,
                                    static_cast<jlong>(resourceOptions.maximumCacheSize()));
    
    // Clean up local references
    env->DeleteLocalRef(apiKey);
    env->DeleteLocalRef(tileServerOptions);
    env->DeleteLocalRef(cachePath);
    env->DeleteLocalRef(assetPath);
    
    return result;
}

} // namespace maplibre_jni