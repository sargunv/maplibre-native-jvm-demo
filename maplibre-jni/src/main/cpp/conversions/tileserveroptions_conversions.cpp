#include "tileserveroptions_conversions.hpp"
#include <stdexcept>

namespace maplibre_jni {

// Static member definitions
jclass TileServerOptionsConversions::tileServerOptionsClass = nullptr;
jfieldID TileServerOptionsConversions::baseURLField = nullptr;
jfieldID TileServerOptionsConversions::uriSchemeAliasField = nullptr;
jfieldID TileServerOptionsConversions::apiKeyParameterNameField = nullptr;
jfieldID TileServerOptionsConversions::requiresApiKeyField = nullptr;
jfieldID TileServerOptionsConversions::sourceTemplateField = nullptr;
jfieldID TileServerOptionsConversions::sourceDomainNameField = nullptr;
jfieldID TileServerOptionsConversions::sourceVersionPrefixField = nullptr;
jfieldID TileServerOptionsConversions::styleTemplateField = nullptr;
jfieldID TileServerOptionsConversions::styleDomainNameField = nullptr;
jfieldID TileServerOptionsConversions::styleVersionPrefixField = nullptr;
jfieldID TileServerOptionsConversions::spritesTemplateField = nullptr;
jfieldID TileServerOptionsConversions::spritesDomainNameField = nullptr;
jfieldID TileServerOptionsConversions::spritesVersionPrefixField = nullptr;
jfieldID TileServerOptionsConversions::glyphsTemplateField = nullptr;
jfieldID TileServerOptionsConversions::glyphsDomainNameField = nullptr;
jfieldID TileServerOptionsConversions::glyphsVersionPrefixField = nullptr;
jfieldID TileServerOptionsConversions::tileTemplateField = nullptr;
jfieldID TileServerOptionsConversions::tileDomainNameField = nullptr;
jfieldID TileServerOptionsConversions::tileVersionPrefixField = nullptr;
jfieldID TileServerOptionsConversions::defaultStyleField = nullptr;
jmethodID TileServerOptionsConversions::constructor = nullptr;
bool TileServerOptionsConversions::initialized = false;

void TileServerOptionsConversions::init(JNIEnv* env) {
    if (initialized) return;
    
    // Find the TileServerOptions class
    jclass localClass = env->FindClass("com/maplibre/jni/TileServerOptions");
    if (!localClass) {
        throw std::runtime_error("Could not find TileServerOptions class");
    }
    
    // Create global reference
    tileServerOptionsClass = (jclass)env->NewGlobalRef(localClass);
    env->DeleteLocalRef(localClass);
    
    // Cache field IDs
    baseURLField = env->GetFieldID(tileServerOptionsClass, "baseURL", "Ljava/lang/String;");
    uriSchemeAliasField = env->GetFieldID(tileServerOptionsClass, "uriSchemeAlias", "Ljava/lang/String;");
    apiKeyParameterNameField = env->GetFieldID(tileServerOptionsClass, "apiKeyParameterName", "Ljava/lang/String;");
    requiresApiKeyField = env->GetFieldID(tileServerOptionsClass, "requiresApiKey", "Z");
    
    sourceTemplateField = env->GetFieldID(tileServerOptionsClass, "sourceTemplate", "Ljava/lang/String;");
    sourceDomainNameField = env->GetFieldID(tileServerOptionsClass, "sourceDomainName", "Ljava/lang/String;");
    sourceVersionPrefixField = env->GetFieldID(tileServerOptionsClass, "sourceVersionPrefix", "Ljava/lang/String;");
    
    styleTemplateField = env->GetFieldID(tileServerOptionsClass, "styleTemplate", "Ljava/lang/String;");
    styleDomainNameField = env->GetFieldID(tileServerOptionsClass, "styleDomainName", "Ljava/lang/String;");
    styleVersionPrefixField = env->GetFieldID(tileServerOptionsClass, "styleVersionPrefix", "Ljava/lang/String;");
    
    spritesTemplateField = env->GetFieldID(tileServerOptionsClass, "spritesTemplate", "Ljava/lang/String;");
    spritesDomainNameField = env->GetFieldID(tileServerOptionsClass, "spritesDomainName", "Ljava/lang/String;");
    spritesVersionPrefixField = env->GetFieldID(tileServerOptionsClass, "spritesVersionPrefix", "Ljava/lang/String;");
    
    glyphsTemplateField = env->GetFieldID(tileServerOptionsClass, "glyphsTemplate", "Ljava/lang/String;");
    glyphsDomainNameField = env->GetFieldID(tileServerOptionsClass, "glyphsDomainName", "Ljava/lang/String;");
    glyphsVersionPrefixField = env->GetFieldID(tileServerOptionsClass, "glyphsVersionPrefix", "Ljava/lang/String;");
    
    tileTemplateField = env->GetFieldID(tileServerOptionsClass, "tileTemplate", "Ljava/lang/String;");
    tileDomainNameField = env->GetFieldID(tileServerOptionsClass, "tileDomainName", "Ljava/lang/String;");
    tileVersionPrefixField = env->GetFieldID(tileServerOptionsClass, "tileVersionPrefix", "Ljava/lang/String;");
    
    defaultStyleField = env->GetFieldID(tileServerOptionsClass, "defaultStyle", "Ljava/lang/String;");
    
    // Cache constructor - all String parameters, requiresApiKey is boolean
    constructor = env->GetMethodID(tileServerOptionsClass, "<init>", 
        "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;ZLjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
    if (!constructor) {
        throw std::runtime_error("Could not find TileServerOptions constructor");
    }
    
    initialized = true;
}

void TileServerOptionsConversions::destroy(JNIEnv* env) {
    if (!initialized) return;
    
    if (tileServerOptionsClass) {
        env->DeleteGlobalRef(tileServerOptionsClass);
        tileServerOptionsClass = nullptr;
    }
    
    initialized = false;
}

mbgl::TileServerOptions TileServerOptionsConversions::extract(JNIEnv* env, jobject tileServerOptions) {
    if (!initialized) {
        init(env);
    }
    
    if (!tileServerOptions) {
        return mbgl::TileServerOptions::DefaultConfiguration();
    }
    
    mbgl::TileServerOptions options;
    
    // Helper to extract string field
    auto extractString = [&](jfieldID field) -> std::string {
        jstring str = (jstring)env->GetObjectField(tileServerOptions, field);
        if (str) {
            const char* cStr = env->GetStringUTFChars(str, nullptr);
            std::string result(cStr);
            env->ReleaseStringUTFChars(str, cStr);
            env->DeleteLocalRef(str);
            return result;
        }
        return "";
    };
    
    // Helper to extract optional string field
    auto extractOptionalString = [&](jfieldID field) -> std::optional<std::string> {
        jstring str = (jstring)env->GetObjectField(tileServerOptions, field);
        if (str) {
            const char* cStr = env->GetStringUTFChars(str, nullptr);
            std::string result(cStr);
            env->ReleaseStringUTFChars(str, cStr);
            env->DeleteLocalRef(str);
            if (!result.empty()) {
                return result;
            }
        }
        return std::nullopt;
    };
    
    // Set basic options
    options.withBaseURL(extractString(baseURLField));
    options.withUriSchemeAlias(extractString(uriSchemeAliasField));
    options.withApiKeyParameterName(extractString(apiKeyParameterNameField));
    options.setRequiresApiKey(env->GetBooleanField(tileServerOptions, requiresApiKeyField));
    
    // Set templates with domain names and version prefixes
    options.withSourceTemplate(
        extractString(sourceTemplateField),
        extractString(sourceDomainNameField),
        extractOptionalString(sourceVersionPrefixField)
    );
    
    options.withStyleTemplate(
        extractString(styleTemplateField),
        extractString(styleDomainNameField),
        extractOptionalString(styleVersionPrefixField)
    );
    
    options.withSpritesTemplate(
        extractString(spritesTemplateField),
        extractString(spritesDomainNameField),
        extractOptionalString(spritesVersionPrefixField)
    );
    
    options.withGlyphsTemplate(
        extractString(glyphsTemplateField),
        extractString(glyphsDomainNameField),
        extractOptionalString(glyphsVersionPrefixField)
    );
    
    options.withTileTemplate(
        extractString(tileTemplateField),
        extractString(tileDomainNameField),
        extractOptionalString(tileVersionPrefixField)
    );
    
    // Set default style
    std::string defaultStyle = extractString(defaultStyleField);
    if (!defaultStyle.empty()) {
        options.withDefaultStyle(defaultStyle);
    }
    
    return options;
}

jobject TileServerOptionsConversions::create(JNIEnv* env, const mbgl::TileServerOptions& tileServerOptions) {
    if (!initialized) {
        init(env);
    }
    
    // Create Java strings for all fields
    jstring baseURL = env->NewStringUTF(tileServerOptions.baseURL().c_str());
    jstring uriSchemeAlias = env->NewStringUTF(tileServerOptions.uriSchemeAlias().c_str());
    jstring apiKeyParameterName = env->NewStringUTF(tileServerOptions.apiKeyParameterName().c_str());
    
    jstring sourceTemplate = env->NewStringUTF(tileServerOptions.sourceTemplate().c_str());
    jstring sourceDomainName = env->NewStringUTF(tileServerOptions.sourceDomainName().c_str());
    jstring sourceVersionPrefix = tileServerOptions.sourceVersionPrefix().has_value() 
        ? env->NewStringUTF(tileServerOptions.sourceVersionPrefix().value().c_str())
        : nullptr;
    
    jstring styleTemplate = env->NewStringUTF(tileServerOptions.styleTemplate().c_str());
    jstring styleDomainName = env->NewStringUTF(tileServerOptions.styleDomainName().c_str());
    jstring styleVersionPrefix = tileServerOptions.styleVersionPrefix().has_value()
        ? env->NewStringUTF(tileServerOptions.styleVersionPrefix().value().c_str())
        : nullptr;
    
    jstring spritesTemplate = env->NewStringUTF(tileServerOptions.spritesTemplate().c_str());
    jstring spritesDomainName = env->NewStringUTF(tileServerOptions.spritesDomainName().c_str());
    jstring spritesVersionPrefix = tileServerOptions.spritesVersionPrefix().has_value()
        ? env->NewStringUTF(tileServerOptions.spritesVersionPrefix().value().c_str())
        : nullptr;
    
    jstring glyphsTemplate = env->NewStringUTF(tileServerOptions.glyphsTemplate().c_str());
    jstring glyphsDomainName = env->NewStringUTF(tileServerOptions.glyphsDomainName().c_str());
    jstring glyphsVersionPrefix = tileServerOptions.glyphsVersionPrefix().has_value()
        ? env->NewStringUTF(tileServerOptions.glyphsVersionPrefix().value().c_str())
        : nullptr;
    
    jstring tileTemplate = env->NewStringUTF(tileServerOptions.tileTemplate().c_str());
    jstring tileDomainName = env->NewStringUTF(tileServerOptions.tileDomainName().c_str());
    jstring tileVersionPrefix = tileServerOptions.tileVersionPrefix().has_value()
        ? env->NewStringUTF(tileServerOptions.tileVersionPrefix().value().c_str())
        : nullptr;
    
    jstring defaultStyle = env->NewStringUTF(tileServerOptions.defaultStyle().c_str());
    
    // Create TileServerOptions object
    jobject result = env->NewObject(tileServerOptionsClass, constructor,
        baseURL, uriSchemeAlias, apiKeyParameterName, tileServerOptions.requiresApiKey(),
        sourceTemplate, sourceDomainName, sourceVersionPrefix,
        styleTemplate, styleDomainName, styleVersionPrefix,
        spritesTemplate, spritesDomainName, spritesVersionPrefix,
        glyphsTemplate, glyphsDomainName, glyphsVersionPrefix,
        tileTemplate, tileDomainName, tileVersionPrefix,
        defaultStyle
    );
    
    // Clean up local references
    env->DeleteLocalRef(baseURL);
    env->DeleteLocalRef(uriSchemeAlias);
    env->DeleteLocalRef(apiKeyParameterName);
    env->DeleteLocalRef(sourceTemplate);
    env->DeleteLocalRef(sourceDomainName);
    if (sourceVersionPrefix) env->DeleteLocalRef(sourceVersionPrefix);
    env->DeleteLocalRef(styleTemplate);
    env->DeleteLocalRef(styleDomainName);
    if (styleVersionPrefix) env->DeleteLocalRef(styleVersionPrefix);
    env->DeleteLocalRef(spritesTemplate);
    env->DeleteLocalRef(spritesDomainName);
    if (spritesVersionPrefix) env->DeleteLocalRef(spritesVersionPrefix);
    env->DeleteLocalRef(glyphsTemplate);
    env->DeleteLocalRef(glyphsDomainName);
    if (glyphsVersionPrefix) env->DeleteLocalRef(glyphsVersionPrefix);
    env->DeleteLocalRef(tileTemplate);
    env->DeleteLocalRef(tileDomainName);
    if (tileVersionPrefix) env->DeleteLocalRef(tileVersionPrefix);
    env->DeleteLocalRef(defaultStyle);
    
    return result;
}

} // namespace maplibre_jni