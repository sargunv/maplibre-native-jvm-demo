#include "jni_mapoptions_conversions.hpp"
#include "jni_size_conversions.hpp"
#include <mbgl/map/mode.hpp>
#include <stdexcept>

namespace maplibre_jni {

// Static member definitions
jclass MapOptionsConversions::mapOptionsClass = nullptr;
jfieldID MapOptionsConversions::mapModeField = nullptr;
jfieldID MapOptionsConversions::constrainModeField = nullptr;
jfieldID MapOptionsConversions::viewportModeField = nullptr;
jfieldID MapOptionsConversions::crossSourceCollisionsField = nullptr;
jfieldID MapOptionsConversions::northOrientationField = nullptr;
jfieldID MapOptionsConversions::sizeField = nullptr;
jfieldID MapOptionsConversions::pixelRatioField = nullptr;
jmethodID MapOptionsConversions::constructor = nullptr;
bool MapOptionsConversions::initialized = false;

void MapOptionsConversions::init(JNIEnv* env) {
    if (initialized) return;
    
    // Find the MapOptions class
    jclass localClass = env->FindClass("com/maplibre/jni/MapOptions");
    if (!localClass) {
        throw std::runtime_error("Could not find MapOptions class");
    }
    
    // Create global reference
    mapOptionsClass = (jclass)env->NewGlobalRef(localClass);
    env->DeleteLocalRef(localClass);
    
    // Cache field IDs
    mapModeField = env->GetFieldID(mapOptionsClass, "mapMode", "Lcom/maplibre/jni/MapMode;");
    if (!mapModeField) {
        throw std::runtime_error("Could not find mapMode field");
    }
    
    constrainModeField = env->GetFieldID(mapOptionsClass, "constrainMode", "Lcom/maplibre/jni/ConstrainMode;");
    if (!constrainModeField) {
        throw std::runtime_error("Could not find constrainMode field");
    }
    
    viewportModeField = env->GetFieldID(mapOptionsClass, "viewportMode", "Lcom/maplibre/jni/ViewportMode;");
    if (!viewportModeField) {
        throw std::runtime_error("Could not find viewportMode field");
    }
    
    crossSourceCollisionsField = env->GetFieldID(mapOptionsClass, "crossSourceCollisions", "Z");
    if (!crossSourceCollisionsField) {
        throw std::runtime_error("Could not find crossSourceCollisions field");
    }
    
    northOrientationField = env->GetFieldID(mapOptionsClass, "northOrientation", "Lcom/maplibre/jni/NorthOrientation;");
    if (!northOrientationField) {
        throw std::runtime_error("Could not find northOrientation field");
    }
    
    sizeField = env->GetFieldID(mapOptionsClass, "size", "Lcom/maplibre/jni/Size;");
    if (!sizeField) {
        throw std::runtime_error("Could not find size field");
    }
    
    pixelRatioField = env->GetFieldID(mapOptionsClass, "pixelRatio", "F");
    if (!pixelRatioField) {
        throw std::runtime_error("Could not find pixelRatio field");
    }
    
    // Cache constructor
    constructor = env->GetMethodID(mapOptionsClass, "<init>", 
        "(Lcom/maplibre/jni/MapMode;Lcom/maplibre/jni/ConstrainMode;Lcom/maplibre/jni/ViewportMode;ZLcom/maplibre/jni/NorthOrientation;Lcom/maplibre/jni/Size;F)V");
    if (!constructor) {
        throw std::runtime_error("Could not find MapOptions constructor");
    }
    
    // Initialize dependent conversions
    SizeConversions::init(env);
    
    initialized = true;
}

void MapOptionsConversions::destroy(JNIEnv* env) {
    if (!initialized) return;
    
    if (mapOptionsClass) {
        env->DeleteGlobalRef(mapOptionsClass);
        mapOptionsClass = nullptr;
    }
    
    mapModeField = nullptr;
    constrainModeField = nullptr;
    viewportModeField = nullptr;
    crossSourceCollisionsField = nullptr;
    northOrientationField = nullptr;
    sizeField = nullptr;
    pixelRatioField = nullptr;
    constructor = nullptr;
    initialized = false;
}

mbgl::MapOptions MapOptionsConversions::extract(JNIEnv* env, jobject mapOptions) {
    if (!initialized) {
        init(env);
    }
    
    if (!mapOptions) {
        return mbgl::MapOptions();
    }
    
    mbgl::MapOptions options;
    
    // Extract mapMode enum
    jobject mapModeObj = env->GetObjectField(mapOptions, mapModeField);
    if (mapModeObj) {
        jclass enumClass = env->GetObjectClass(mapModeObj);
        jfieldID nativeValueField = env->GetFieldID(enumClass, "nativeValue", "I");
        jint mapMode = env->GetIntField(mapModeObj, nativeValueField);
        options.withMapMode(static_cast<mbgl::MapMode>(mapMode));
        env->DeleteLocalRef(mapModeObj);
        env->DeleteLocalRef(enumClass);
    }
    
    // Extract constrainMode enum
    jobject constrainModeObj = env->GetObjectField(mapOptions, constrainModeField);
    if (constrainModeObj) {
        jclass enumClass = env->GetObjectClass(constrainModeObj);
        jfieldID nativeValueField = env->GetFieldID(enumClass, "nativeValue", "I");
        jint constrainMode = env->GetIntField(constrainModeObj, nativeValueField);
        options.withConstrainMode(static_cast<mbgl::ConstrainMode>(constrainMode));
        env->DeleteLocalRef(constrainModeObj);
        env->DeleteLocalRef(enumClass);
    }
    
    // Extract viewportMode enum
    jobject viewportModeObj = env->GetObjectField(mapOptions, viewportModeField);
    if (viewportModeObj) {
        jclass enumClass = env->GetObjectClass(viewportModeObj);
        jfieldID nativeValueField = env->GetFieldID(enumClass, "nativeValue", "I");
        jint viewportMode = env->GetIntField(viewportModeObj, nativeValueField);
        options.withViewportMode(static_cast<mbgl::ViewportMode>(viewportMode));
        env->DeleteLocalRef(viewportModeObj);
        env->DeleteLocalRef(enumClass);
    }
    
    // Extract crossSourceCollisions boolean
    jboolean crossSourceCollisions = env->GetBooleanField(mapOptions, crossSourceCollisionsField);
    options.withCrossSourceCollisions(crossSourceCollisions);
    
    // Extract northOrientation enum
    jobject northOrientationObj = env->GetObjectField(mapOptions, northOrientationField);
    if (northOrientationObj) {
        jclass enumClass = env->GetObjectClass(northOrientationObj);
        jfieldID nativeValueField = env->GetFieldID(enumClass, "nativeValue", "I");
        jint northOrientation = env->GetIntField(northOrientationObj, nativeValueField);
        options.withNorthOrientation(static_cast<mbgl::NorthOrientation>(northOrientation));
        env->DeleteLocalRef(northOrientationObj);
        env->DeleteLocalRef(enumClass);
    }
    
    // Extract size
    jobject sizeObj = env->GetObjectField(mapOptions, sizeField);
    if (sizeObj) {
        options.withSize(SizeConversions::extract(env, sizeObj));
        env->DeleteLocalRef(sizeObj);
    }
    
    // Extract pixelRatio
    jfloat pixelRatio = env->GetFloatField(mapOptions, pixelRatioField);
    options.withPixelRatio(pixelRatio);
    
    return options;
}

jobject MapOptionsConversions::create(JNIEnv* env, const mbgl::MapOptions& mapOptions) {
    if (!initialized) {
        init(env);
    }
    
    // Create MapMode enum
    jclass mapModeClass = env->FindClass("com/maplibre/jni/MapMode");
    jmethodID fromNative = env->GetStaticMethodID(mapModeClass, "fromNative", "(I)Lcom/maplibre/jni/MapMode;");
    jobject mapMode = env->CallStaticObjectMethod(mapModeClass, fromNative, static_cast<jint>(mapOptions.mapMode()));
    
    // Create ConstrainMode enum
    jclass constrainModeClass = env->FindClass("com/maplibre/jni/ConstrainMode");
    fromNative = env->GetStaticMethodID(constrainModeClass, "fromNative", "(I)Lcom/maplibre/jni/ConstrainMode;");
    jobject constrainMode = env->CallStaticObjectMethod(constrainModeClass, fromNative, static_cast<jint>(mapOptions.constrainMode()));
    
    // Create ViewportMode enum
    jclass viewportModeClass = env->FindClass("com/maplibre/jni/ViewportMode");
    fromNative = env->GetStaticMethodID(viewportModeClass, "fromNative", "(I)Lcom/maplibre/jni/ViewportMode;");
    jobject viewportMode = env->CallStaticObjectMethod(viewportModeClass, fromNative, static_cast<jint>(mapOptions.viewportMode()));
    
    // Create NorthOrientation enum
    jclass northOrientationClass = env->FindClass("com/maplibre/jni/NorthOrientation");
    fromNative = env->GetStaticMethodID(northOrientationClass, "fromNative", "(I)Lcom/maplibre/jni/NorthOrientation;");
    jobject northOrientation = env->CallStaticObjectMethod(northOrientationClass, fromNative, static_cast<jint>(mapOptions.northOrientation()));
    
    // Create Size object
    jobject size = SizeConversions::create(env, mapOptions.size());
    
    // Create MapOptions object
    jobject result = env->NewObject(mapOptionsClass, constructor,
                                    mapMode, constrainMode, viewportMode,
                                    mapOptions.crossSourceCollisions(),
                                    northOrientation, size, mapOptions.pixelRatio());
    
    // Clean up local references
    env->DeleteLocalRef(mapModeClass);
    env->DeleteLocalRef(constrainModeClass);
    env->DeleteLocalRef(viewportModeClass);
    env->DeleteLocalRef(northOrientationClass);
    env->DeleteLocalRef(mapMode);
    env->DeleteLocalRef(constrainMode);
    env->DeleteLocalRef(viewportMode);
    env->DeleteLocalRef(northOrientation);
    env->DeleteLocalRef(size);
    
    return result;
}

} // namespace maplibre_jni