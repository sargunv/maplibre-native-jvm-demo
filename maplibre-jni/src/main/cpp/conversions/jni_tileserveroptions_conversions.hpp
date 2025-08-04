#pragma once

#include <jni.h>
#include <mbgl/util/tile_server_options.hpp>

namespace maplibre_jni {

class TileServerOptionsConversions {
public:
    static void init(JNIEnv* env);
    static void destroy(JNIEnv* env);
    
    // Extract mbgl::TileServerOptions from Java TileServerOptions object
    static mbgl::TileServerOptions extract(JNIEnv* env, jobject tileServerOptions);
    
    // Create Java TileServerOptions object from mbgl::TileServerOptions
    static jobject create(JNIEnv* env, const mbgl::TileServerOptions& tileServerOptions);
    
private:
    static jclass tileServerOptionsClass;
    static jfieldID baseURLField;
    static jfieldID uriSchemeAliasField;
    static jfieldID apiKeyParameterNameField;
    static jfieldID requiresApiKeyField;
    static jfieldID sourceTemplateField;
    static jfieldID sourceDomainNameField;
    static jfieldID sourceVersionPrefixField;
    static jfieldID styleTemplateField;
    static jfieldID styleDomainNameField;
    static jfieldID styleVersionPrefixField;
    static jfieldID spritesTemplateField;
    static jfieldID spritesDomainNameField;
    static jfieldID spritesVersionPrefixField;
    static jfieldID glyphsTemplateField;
    static jfieldID glyphsDomainNameField;
    static jfieldID glyphsVersionPrefixField;
    static jfieldID tileTemplateField;
    static jfieldID tileDomainNameField;
    static jfieldID tileVersionPrefixField;
    static jfieldID defaultStyleField;
    static jmethodID constructor;
    static bool initialized;
};

} // namespace maplibre_jni