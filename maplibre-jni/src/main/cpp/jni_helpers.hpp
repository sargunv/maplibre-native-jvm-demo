#pragma once

#include <jni.h>
#include <string>

template<typename T>
inline T* fromJavaPointer(jlong ptr) {
    return reinterpret_cast<T*>(ptr);
}

template<typename T>
inline jlong toJavaPointer(T* ptr) {
    return reinterpret_cast<jlong>(ptr);
}

inline void throwJavaException(JNIEnv* env, const char* className, const std::string& msg) {
    jclass exClass = env->FindClass(className);
    if (exClass != nullptr) {
        env->ThrowNew(exClass, msg.c_str());
    }
}