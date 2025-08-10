#include "jni_renderer_frontend.hpp"
#include "jni_helpers.hpp"
#include <mbgl/renderer/renderer_observer.hpp>
#include <mbgl/renderer/update_parameters.hpp>
#include <mbgl/renderer/renderer.hpp>
#include <stdexcept>

using namespace mbgl;

namespace maplibre_jni {

static jclass gFrontendCls = nullptr;
static jmethodID gResetMID = nullptr;
static jmethodID gOnUpdateMID = nullptr; // void onUpdateFromNative(long paramsId)
static jmethodID gGetThreadPoolPtrMID = nullptr; // long getThreadPoolNativePtr()

JniRendererFrontend::JniRendererFrontend(JNIEnv* env, jobject kotlinFrontendObject) {
    // Dedicated RunLoop to drive mbgl tasks on AWT thread
    runLoop = std::make_unique<mbgl::util::RunLoop>(mbgl::util::RunLoop::Type::New);
    env->GetJavaVM(&jvm);
    kotlinFrontendGlobal = env->NewGlobalRef(kotlinFrontendObject);

    if (!gFrontendCls) {
        jclass local = env->GetObjectClass(kotlinFrontendObject);
        gFrontendCls = reinterpret_cast<jclass>(env->NewGlobalRef(local));
        env->DeleteLocalRef(local);
        gResetMID = env->GetMethodID(gFrontendCls, "reset", "()V");
        gOnUpdateMID = env->GetMethodID(gFrontendCls, "onUpdateFromNative", "(J)V");
        gGetThreadPoolPtrMID = env->GetMethodID(gFrontendCls, "getThreadPoolNativePtr", "()J");
    }
}

JniRendererFrontend::~JniRendererFrontend() {
    JNIEnv* env = getEnv();
    if (env && kotlinFrontendGlobal) {
        env->DeleteGlobalRef(kotlinFrontendGlobal);
        kotlinFrontendGlobal = nullptr;
    }
}

JNIEnv* JniRendererFrontend::getEnv() const {
    JNIEnv* env = nullptr;
    if (!jvm) return nullptr;
    jint res = jvm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6);
    if (res == JNI_EDETACHED) {
        if (jvm->AttachCurrentThread(reinterpret_cast<void**>(&env), nullptr) != JNI_OK) return nullptr;
    } else if (res != JNI_OK) {
        return nullptr;
    }
    return env;
}

void JniRendererFrontend::callKotlinReset(JNIEnv* env) const {
    env->CallVoidMethod(kotlinFrontendGlobal, gResetMID);
}

void JniRendererFrontend::callKotlinOnUpdate(JNIEnv* env, jlong paramsId) const {
    env->CallVoidMethod(kotlinFrontendGlobal, gOnUpdateMID, paramsId);
}

jlong JniRendererFrontend::callKotlinGetThreadPoolPtr(JNIEnv* env) const {
    return env->CallLongMethod(kotlinFrontendGlobal, gGetThreadPoolPtrMID);
}

void JniRendererFrontend::reset() {
    std::lock_guard<std::mutex> lock(mutex);
    registry.clear();
    if (JNIEnv* env = getEnv()) {
        callKotlinReset(env);
    }
}

void JniRendererFrontend::setObserver(RendererObserver& o) {
    observer = &o;
}

void JniRendererFrontend::update(std::shared_ptr<UpdateParameters> params) {
    uint64_t id;
    {
        std::lock_guard<std::mutex> lock(mutex);
        id = nextId++;
        registry[id] = std::move(params);
    }
    if (JNIEnv* env = getEnv()) {
        callKotlinOnUpdate(env, static_cast<jlong>(id));
    }
}

const TaggedScheduler& JniRendererFrontend::getThreadPool() const {
    if (cachedThreadPool) return *cachedThreadPool;
    if (JNIEnv* env = getEnv()) {
        jlong ptr = callKotlinGetThreadPoolPtr(env);
        cachedThreadPool = reinterpret_cast<TaggedScheduler*>(ptr);
    }
    if (!cachedThreadPool) throw std::runtime_error("ThreadPool not available");
    return *cachedThreadPool;
}

void JniRendererFrontend::renderWithParams(Renderer& renderer, uint64_t paramsId) {
    std::shared_ptr<UpdateParameters> params;
    {
        std::lock_guard<std::mutex> lock(mutex);
        auto it = registry.find(paramsId);
        if (it == registry.end()) throw std::runtime_error("Invalid UpdateParameters id");
        params = it->second;
    }
    renderer.render(params);
}

extern "C" jlong Java_org_maplibre_kmp_native_internal_Native_createJniRendererFrontend(JNIEnv* env, jclass, jobject kotlinFrontend) {
    try {
        auto* frontend = new JniRendererFrontend(env, kotlinFrontend);
        return toJavaPointer(frontend);
    } catch (const std::exception& e) {
        throwJavaException(env, "java/lang/RuntimeException", e.what());
        return 0;
    }
}

extern "C" void Java_org_maplibre_kmp_native_internal_Native_destroyJniRendererFrontend(JNIEnv* env, jclass, jlong ptr) {
    delete fromJavaPointer<JniRendererFrontend>(ptr);
}

extern "C" void Java_org_maplibre_kmp_native_internal_Native_runLoopOnce(JNIEnv* env, jclass, jlong frontendPtr) {
    try {
        auto* frontend = fromJavaPointer<JniRendererFrontend>(frontendPtr);
        frontend->runLoopOnce();
    } catch (const std::exception& e) {
        throwJavaException(env, "java/lang/RuntimeException", e.what());
    }
}

} // namespace maplibre_jni
