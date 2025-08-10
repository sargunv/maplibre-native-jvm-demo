#include "edgeinsets_conversions.hpp"
#include <stdexcept>

namespace maplibre_jni
{

    // Static member definitions
    jclass EdgeInsetsConversions::edgeInsetsClass = nullptr;
    jfieldID EdgeInsetsConversions::topField = nullptr;
    jfieldID EdgeInsetsConversions::leftField = nullptr;
    jfieldID EdgeInsetsConversions::bottomField = nullptr;
    jfieldID EdgeInsetsConversions::rightField = nullptr;
    jmethodID EdgeInsetsConversions::constructor = nullptr;
    bool EdgeInsetsConversions::initialized = false;

    void EdgeInsetsConversions::init(JNIEnv *env)
    {
        if (initialized)
            return;

        // Find the EdgeInsets class
        jclass localClass = env->FindClass("org/maplibre/kmp/native/EdgeInsets");
        if (!localClass)
        {
            throw std::runtime_error("Could not find EdgeInsets class");
        }

        // Create global reference
        edgeInsetsClass = (jclass)env->NewGlobalRef(localClass);
        env->DeleteLocalRef(localClass);

        // Cache field IDs
        topField = env->GetFieldID(edgeInsetsClass, "top", "D");
        if (!topField)
        {
            throw std::runtime_error("Could not find top field");
        }

        leftField = env->GetFieldID(edgeInsetsClass, "left", "D");
        if (!leftField)
        {
            throw std::runtime_error("Could not find left field");
        }

        bottomField = env->GetFieldID(edgeInsetsClass, "bottom", "D");
        if (!bottomField)
        {
            throw std::runtime_error("Could not find bottom field");
        }

        rightField = env->GetFieldID(edgeInsetsClass, "right", "D");
        if (!rightField)
        {
            throw std::runtime_error("Could not find right field");
        }

        // Cache constructor
        constructor = env->GetMethodID(edgeInsetsClass, "<init>", "(DDDD)V");
        if (!constructor)
        {
            throw std::runtime_error("Could not find EdgeInsets constructor");
        }

        initialized = true;
    }

    void EdgeInsetsConversions::destroy(JNIEnv *env)
    {
        if (!initialized)
            return;

        if (edgeInsetsClass)
        {
            env->DeleteGlobalRef(edgeInsetsClass);
            edgeInsetsClass = nullptr;
        }

        topField = nullptr;
        leftField = nullptr;
        bottomField = nullptr;
        rightField = nullptr;
        constructor = nullptr;
        initialized = false;
    }

    mbgl::EdgeInsets EdgeInsetsConversions::extract(JNIEnv *env, jobject edgeInsets)
    {
        if (!initialized)
        {
            init(env);
        }

        if (!edgeInsets)
        {
            throw std::invalid_argument("EdgeInsets object is null");
        }

        double top = env->GetDoubleField(edgeInsets, topField);
        double left = env->GetDoubleField(edgeInsets, leftField);
        double bottom = env->GetDoubleField(edgeInsets, bottomField);
        double right = env->GetDoubleField(edgeInsets, rightField);

        return mbgl::EdgeInsets{top, left, bottom, right};
    }

    jobject EdgeInsetsConversions::create(JNIEnv *env, const mbgl::EdgeInsets &edgeInsets)
    {
        if (!initialized)
        {
            init(env);
        }

        return env->NewObject(edgeInsetsClass, constructor,
                              edgeInsets.top(),
                              edgeInsets.left(),
                              edgeInsets.bottom(),
                              edgeInsets.right());
    }

} // namespace maplibre_jni
