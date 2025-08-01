#include "generated/kotlin/main/com_maplibre_jni_RunLoop.h"
#include "jni_helpers.hpp"
#include <mbgl/util/run_loop.hpp>
#include <memory>

// Store the RunLoop instance - MapLibre uses thread-local storage internally
// so we need to ensure the RunLoop is created on the correct thread
static std::unique_ptr<mbgl::util::RunLoop> globalRunLoop;

extern "C" {

JNIEXPORT jlong JNICALL Java_com_maplibre_jni_RunLoop_nativeCreateDefault
  (JNIEnv* env, jclass) {
    try {
        // Create a default RunLoop for this thread
        // This sets the RunLoop as the current scheduler for this thread
        auto* runLoop = new mbgl::util::RunLoop(mbgl::util::RunLoop::Type::Default);
        
        // Store it globally for now (single-threaded assumption)
        // In a multi-threaded scenario, you'd use thread-local storage
        globalRunLoop.reset(runLoop);
        
        return toJavaPointer(runLoop);
    } catch (const std::exception& e) {
        throwJavaException(env, "java/lang/RuntimeException", e.what());
        return 0;
    }
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_RunLoop_nativeDestroy
  (JNIEnv* env, jclass, jlong ptr) {
    try {
        auto* runLoop = fromJavaPointer<mbgl::util::RunLoop>(ptr);
        
        // Clear the global reference if it matches
        if (globalRunLoop.get() == runLoop) {
            globalRunLoop.reset();
        } else {
            delete runLoop;
        }
        
    } catch (const std::exception& e) {
        throwJavaException(env, "java/lang/RuntimeException", e.what());
    }
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_RunLoop_nativeRunOnce
  (JNIEnv* env, jclass, jlong ptr) {
    try {
        auto* runLoop = fromJavaPointer<mbgl::util::RunLoop>(ptr);
        
        // Process pending events without blocking
        // This is the key method that processes async callbacks
        runLoop->runOnce();
        
    } catch (const std::exception& e) {
        throwJavaException(env, "java/lang/RuntimeException", e.what());
    }
}

JNIEXPORT void JNICALL Java_com_maplibre_jni_RunLoop_nativeStop
  (JNIEnv* env, jclass, jlong ptr) {
    try {
        auto* runLoop = fromJavaPointer<mbgl::util::RunLoop>(ptr);
        runLoop->stop();
    } catch (const std::exception& e) {
        throwJavaException(env, "java/lang/RuntimeException", e.what());
    }
}

}