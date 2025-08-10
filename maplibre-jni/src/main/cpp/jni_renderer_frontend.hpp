#pragma once

#include <jni.h>
#include <memory>
#include <unordered_map>
#include <atomic>
#include <mutex>
#include <mbgl/renderer/renderer_frontend.hpp>
#include <mbgl/renderer/renderer_observer.hpp>
#include <mbgl/gfx/rendering_stats.hpp>
#include <mbgl/renderer/renderer.hpp>
#include <mbgl/util/run_loop.hpp>

namespace maplibre_jni {

class JniRendererFrontend : public mbgl::RendererFrontend, public mbgl::RendererObserver {
public:
    JniRendererFrontend(JNIEnv* env, jobject kotlinFrontendObject);
    ~JniRendererFrontend() override;

    void reset() override;
    void setObserver(mbgl::RendererObserver& observer) override;
    void update(std::shared_ptr<mbgl::UpdateParameters> params) override;
    const mbgl::TaggedScheduler& getThreadPool() const override;

    // Render using a params id stored in the registry
    void renderWithParams(mbgl::Renderer& renderer, uint64_t paramsId);

    // RendererObserver forwarders
    void onInvalidate() override { if (observer) observer->onInvalidate(); }
    void onWillStartRenderingFrame() override { if (observer) observer->onWillStartRenderingFrame(); }
    void onDidFinishRenderingFrame(mbgl::RendererObserver::RenderMode m, bool r, bool p, const mbgl::gfx::RenderingStats& stats) override { if (observer) observer->onDidFinishRenderingFrame(m, r, p, stats); }
    void onWillStartRenderingMap() override { if (observer) observer->onWillStartRenderingMap(); }
    void onDidFinishRenderingMap() override { if (observer) observer->onDidFinishRenderingMap(); }

public:
    // Pump the internal RunLoop once (network/timer callbacks)
    void runLoopOnce() { if (runLoop) runLoop->runOnce(); }

private:
    JNIEnv* getEnv() const;
    void callKotlinReset(JNIEnv* env) const;
    void callKotlinOnUpdate(JNIEnv* env, jlong paramsId) const;
    jlong callKotlinGetThreadPoolPtr(JNIEnv* env) const;

    JavaVM* jvm{nullptr};
    jobject kotlinFrontendGlobal{nullptr};

    mbgl::RendererObserver* observer{nullptr};

    mutable std::mutex mutex;
    std::unordered_map<uint64_t, std::shared_ptr<mbgl::UpdateParameters>> registry;
    std::atomic<uint64_t> nextId{1};

    mutable const mbgl::TaggedScheduler* cachedThreadPool{nullptr};

    // Dedicated RunLoop to drive mbgl tasks when embedded in AWT
    std::unique_ptr<mbgl::util::RunLoop> runLoop;
};

// JNI factories
extern "C" jlong Java_org_maplibre_kmp_native_internal_Native_createJniRendererFrontend(JNIEnv* env, jclass, jobject kotlinFrontend);
extern "C" void Java_org_maplibre_kmp_native_internal_Native_destroyJniRendererFrontend(JNIEnv* env, jclass, jlong ptr);

} // namespace maplibre_jni
