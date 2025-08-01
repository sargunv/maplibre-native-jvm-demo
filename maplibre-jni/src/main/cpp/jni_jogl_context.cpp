#include "generated/kotlin/main/com_maplibre_jni_JOGLContext.h"
#include "jni_helpers.hpp"

#ifdef _WIN32
#include <windows.h>
#include <GL/gl.h>
// Windows: wglGetProcAddress
extern "C" {
    typedef void* (__stdcall *PFNWGLGETPROCADDRESSPROC)(const char*);
    PFNWGLGETPROCADDRESSPROC wglGetProcAddress;
}
#elif defined(__linux__)
#include <GL/glx.h>
#include <dlfcn.h>
// Linux: glXGetProcAddress
#elif defined(__APPLE__)
#include <dlfcn.h>
// macOS: dlsym
#endif

extern "C" {

JNIEXPORT jlong JNICALL Java_com_maplibre_jni_JOGLContext_nativeGetProcAddress
  (JNIEnv* env, jclass, jstring jName) {
    const char* name = env->GetStringUTFChars(jName, nullptr);
    if (!name) {
        return 0;
    }
    
    void* address = nullptr;
    
#ifdef _WIN32
    // On Windows, try wglGetProcAddress first
    static HMODULE opengl32 = LoadLibraryA("opengl32.dll");
    if (!wglGetProcAddress && opengl32) {
        wglGetProcAddress = (PFNWGLGETPROCADDRESSPROC)GetProcAddress(opengl32, "wglGetProcAddress");
    }
    
    if (wglGetProcAddress) {
        address = wglGetProcAddress(name);
    }
    
    // For OpenGL 1.1 functions, we need to use GetProcAddress on opengl32.dll
    if (!address && opengl32) {
        address = GetProcAddress(opengl32, name);
    }
#elif defined(__linux__)
    // On Linux, use glXGetProcAddress or glXGetProcAddressARB
    typedef void* (*PFNGLXGETPROCADDRESSPROC)(const GLubyte*);
    static PFNGLXGETPROCADDRESSPROC glXGetProcAddress = nullptr;
    
    if (!glXGetProcAddress) {
        // Try to load glXGetProcAddress or glXGetProcAddressARB
        void* libGL = dlopen("libGL.so.1", RTLD_LAZY);
        if (!libGL) {
            libGL = dlopen("libGL.so", RTLD_LAZY);
        }
        
        if (libGL) {
            glXGetProcAddress = (PFNGLXGETPROCADDRESSPROC)dlsym(libGL, "glXGetProcAddress");
            if (!glXGetProcAddress) {
                glXGetProcAddress = (PFNGLXGETPROCADDRESSPROC)dlsym(libGL, "glXGetProcAddressARB");
            }
        }
    }
    
    if (glXGetProcAddress) {
        address = glXGetProcAddress(reinterpret_cast<const GLubyte*>(name));
    }
#elif defined(__APPLE__)
    // On macOS, use dlsym
    static void* framework = dlopen("/System/Library/Frameworks/OpenGL.framework/OpenGL", RTLD_LAZY);
    if (framework) {
        address = dlsym(framework, name);
    }
#endif
    
    env->ReleaseStringUTFChars(jName, name);
    
    return reinterpret_cast<jlong>(address);
}

}