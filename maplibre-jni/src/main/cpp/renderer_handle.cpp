#include "jni_helpers.hpp"
#include "jni_renderer_frontend.hpp"
#include <mbgl/renderer/renderer.hpp>
#include <mbgl/renderer/update_parameters.hpp>
#include <mbgl/gfx/backend_scope.hpp>
#include <mbgl/gfx/renderer_backend.hpp>
#include <jni.h>

using namespace mbgl;
using namespace maplibre_jni;

extern "C" {

JNIEXPORT jlong JNICALL Java_org_maplibre_kmp_native_internal_Native_rendererCreate(JNIEnv* env, jclass, jlong backendPtr, jfloat pixelRatio, jstring localFontFamily) {
    try {
        auto* backend = reinterpret_cast<mbgl::gfx::RendererBackend*>(backendPtr);
        std::optional<std::string> font;
        if (localFontFamily) {
            const char* s = env->GetStringUTFChars(localFontFamily, nullptr);
            font = std::string(s);
            env->ReleaseStringUTFChars(localFontFamily, s);
        }
        auto* renderer = new Renderer(*backend, pixelRatio, font);
        return toJavaPointer(renderer);
    } catch (const std::exception& e) {
        throwJavaException(env, "java/lang/RuntimeException", e.what());
        return 0;
    }
}

JNIEXPORT void JNICALL Java_org_maplibre_kmp_native_internal_Native_rendererDestroy(JNIEnv* env, jclass, jlong rendererPtr) {
    delete fromJavaPointer<Renderer>(rendererPtr);
}

JNIEXPORT void JNICALL Java_org_maplibre_kmp_native_internal_Native_rendererRenderWithFrontend(JNIEnv* env, jclass, jlong rendererPtr, jlong frontendPtr, jlong paramsId) {
    try {
        auto* renderer = fromJavaPointer<Renderer>(rendererPtr);
        auto* frontend = fromJavaPointer<JniRendererFrontend>(frontendPtr);
        frontend->renderWithParams(*renderer, static_cast<uint64_t>(paramsId));
    } catch (const std::exception& e) {
        throwJavaException(env, "java/lang/RuntimeException", e.what());
    }
}

JNIEXPORT void JNICALL Java_org_maplibre_kmp_native_internal_Native_rendererRenderWithBackend(JNIEnv* env, jclass, jlong rendererPtr, jlong frontendPtr, jlong backendPtr, jlong paramsId) {
    try {
        auto* renderer = fromJavaPointer<Renderer>(rendererPtr);
        auto* frontend = fromJavaPointer<JniRendererFrontend>(frontendPtr);
        auto* backend = reinterpret_cast<mbgl::gfx::RendererBackend*>(backendPtr);
        mbgl::gfx::BackendScope scope(*backend);
        frontend->renderWithParams(*renderer, static_cast<uint64_t>(paramsId));
    } catch (const std::exception& e) {
        throwJavaException(env, "java/lang/RuntimeException", e.what());
    }
}

JNIEXPORT void JNICALL Java_org_maplibre_kmp_native_internal_Native_rendererBindObserver(JNIEnv* env, jclass, jlong rendererPtr, jlong frontendPtr) {
    try {
        auto* renderer = fromJavaPointer<Renderer>(rendererPtr);
        auto* frontend = fromJavaPointer<JniRendererFrontend>(frontendPtr);
        renderer->setObserver(frontend);
    } catch (const std::exception& e) {
        throwJavaException(env, "java/lang/RuntimeException", e.what());
    }
}

}
