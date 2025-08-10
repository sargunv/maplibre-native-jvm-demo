#include "awt_canvas_renderer.hpp"
#include "jni_helpers.hpp"
#include "awt_backend_factory.hpp"

#include <mbgl/actor/scheduler.hpp>
#include <mbgl/gfx/backend_scope.hpp>
#include <mbgl/gfx/renderer_backend.hpp>
#include <mbgl/gfx/renderable.hpp>
#include <mbgl/renderer/renderer.hpp>
#include <mbgl/renderer/renderer_observer.hpp>
#include <mbgl/util/async_task.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/util/run_loop.hpp>

#include <jawt.h>
#include <jawt_md.h>
#include <memory>
#include <atomic>

namespace maplibre_jni
{

    // Implementation class that combines frontend and backend functionality
    class AwtCanvasRenderer::Impl : public mbgl::RendererObserver
    {
    public:
        Impl(JNIEnv *env,
             jobject canvas,
             int width,
             int height,
             float pixelRatio,
             const std::optional<std::string> &localFontFamily)
            : runLoop(std::make_unique<mbgl::util::RunLoop>(mbgl::util::RunLoop::Type::New)),
              jvm(nullptr),
              canvasRef(nullptr),
              dirty(false)
        {

            // Get JavaVM for later thread attachment
            env->GetJavaVM(&jvm);

            // Create global reference to canvas
            canvasRef = env->NewGlobalRef(canvas);

            // Create platform-specific backend (Metal on macOS, OpenGL ES on Linux)
            backend = createPlatformBackend(env, canvas, width, height, mbgl::gfx::ContextMode::Unique);

            // Create the renderer with backend
            renderer = std::make_unique<mbgl::Renderer>(
                *backend,
                pixelRatio,
                localFontFamily);

            // Set ourselves as the observer to track invalidation
            renderer->setObserver(this);

            mbgl::Log::Info(mbgl::Event::General, "AwtCanvasRenderer initialized");
        }

        ~Impl()
        {
            // Clean up in reverse order
            renderer.reset();
            backend.reset();
            runLoop.reset();

            // Release global references
            if (canvasRef && jvm)
            {
                JNIEnv *env = getEnv();
                if (env)
                {
                    env->DeleteGlobalRef(canvasRef);
                }
            }
        }

        bool tick()
        {
            // Process RunLoop events (network callbacks, timers, etc.)
            runLoop->runOnce();

            // Check if we need to render
            if (dirty.exchange(false))
            {
                // Render the frame
                mbgl::gfx::BackendScope scope(*backend);
                if (updateParameters)
                {
                    renderer->render(updateParameters);
                }

                // Swap buffers (platform-specific)
                swapBuffers();

                return true; // Did render
            }

            return false; // Nothing to render
        }

        void updateSize(int width, int height)
        {
            // Update the backend size directly - no cast needed!
            backend->setSize(mbgl::Size{static_cast<uint32_t>(width), static_cast<uint32_t>(height)});

            // Mark as dirty to trigger render
            dirty = true;
        }

        void reset()
        {
            renderer.reset();
        }

        void setObserver(mbgl::RendererObserver &observer)
        {
            externalObserver = &observer;
        }

        void update(std::shared_ptr<mbgl::UpdateParameters> parameters)
        {
            // Store the update parameters for the next render
            updateParameters = std::move(parameters);
            // Mark dirty when map state changes
            dirty = true;
        }

        const mbgl::TaggedScheduler &getThreadPool() const
        {
            return backend->getThreadPool();
        }

        mbgl::gfx::RendererBackend *getRendererBackend()
        {
            return backend.get();
        }

        // RendererObserver implementation
        void onInvalidate() override
        {
            // Map needs to be redrawn
            dirty = true;

            // Forward to external observer if set
            if (externalObserver)
            {
                externalObserver->onInvalidate();
            }
        }

        void onResourceError(std::exception_ptr err) override
        {
            if (externalObserver)
            {
                externalObserver->onResourceError(err);
            }
        }

        void onDidFinishRenderingFrame(RenderMode mode, bool repaintNeeded, bool placementChanged, const mbgl::gfx::RenderingStats &stats) override
        {
            if (repaintNeeded)
            {
                dirty = true;
            }

            if (externalObserver)
            {
                externalObserver->onDidFinishRenderingFrame(mode, repaintNeeded, placementChanged, stats);
            }
        }

        void onStyleImageMissing(const std::string &image, const StyleImageMissingCallback &callback) override
        {
            if (externalObserver)
            {
                externalObserver->onStyleImageMissing(image, callback);
            }
        }

    private:
        JNIEnv *getEnv()
        {
            JNIEnv *env = nullptr;
            if (jvm)
            {
                jvm->AttachCurrentThread((void **)&env, nullptr);
            }
            return env;
        }

        void swapBuffers()
        {
            // Platform-specific buffer swap
            // The backend will handle this through its platform-specific implementation
        }

        // Core components
        std::unique_ptr<mbgl::util::RunLoop> runLoop;
        std::unique_ptr<PlatformBackend> backend;
        std::unique_ptr<mbgl::Renderer> renderer;

        // JNI references
        JavaVM *jvm;
        jobject canvasRef;

        // State
        std::atomic<bool> dirty;
        std::shared_ptr<mbgl::UpdateParameters> updateParameters;

        // External observer (usually the Map)
        mbgl::RendererObserver *externalObserver = nullptr;
    };

    // Public interface implementation

    AwtCanvasRenderer::AwtCanvasRenderer() = default;

    AwtCanvasRenderer::~AwtCanvasRenderer() = default;

    std::unique_ptr<AwtCanvasRenderer> AwtCanvasRenderer::create(
        JNIEnv *env,
        jobject canvas,
        int width,
        int height,
        float pixelRatio,
        const std::optional<std::string> &localFontFamily)
    {

        auto renderer = std::unique_ptr<AwtCanvasRenderer>(new AwtCanvasRenderer());
        renderer->impl = std::make_unique<Impl>(env, canvas, width, height, pixelRatio, localFontFamily);
        return renderer;
    }

    bool AwtCanvasRenderer::tick()
    {
        return impl->tick();
    }

    void AwtCanvasRenderer::updateSize(int width, int height)
    {
        impl->updateSize(width, height);
    }

    void AwtCanvasRenderer::reset()
    {
        impl->reset();
    }

    void AwtCanvasRenderer::setObserver(mbgl::RendererObserver &observer)
    {
        impl->setObserver(observer);
    }

    void AwtCanvasRenderer::update(std::shared_ptr<mbgl::UpdateParameters> parameters)
    {
        impl->update(std::move(parameters));
    }

    const mbgl::TaggedScheduler &AwtCanvasRenderer::getThreadPool() const
    {
        return impl->getThreadPool();
    }

    mbgl::gfx::RendererBackend *AwtCanvasRenderer::getRendererBackend()
    {
        return impl->getRendererBackend();
    }

} // namespace maplibre_jni

// JNI bindings
extern "C"
{

    JNIEXPORT jlong JNICALL Java_org_maplibre_kmp_native_AwtCanvasRenderer_nativeCreate(
        JNIEnv *env, jclass,
        jobject canvas, jint width, jint height, jfloat pixelRatio)
    {
        try
        {
            auto renderer = maplibre_jni::AwtCanvasRenderer::create(
                env, canvas, width, height, pixelRatio, std::nullopt);
            return toJavaPointer(renderer.release());
        }
        catch (const std::exception &e)
        {
            throwJavaException(env, "java/lang/RuntimeException", e.what());
            return 0;
        }
    }

    JNIEXPORT void JNICALL Java_org_maplibre_kmp_native_AwtCanvasRenderer_nativeDestroy(
        JNIEnv *env, jclass, jlong ptr)
    {
        delete fromJavaPointer<maplibre_jni::AwtCanvasRenderer>(ptr);
    }

    JNIEXPORT jboolean JNICALL Java_org_maplibre_kmp_native_AwtCanvasRenderer_nativeTick(
        JNIEnv *env, jclass, jlong ptr)
    {
        try
        {
            auto *renderer = fromJavaPointer<maplibre_jni::AwtCanvasRenderer>(ptr);
            return renderer->tick() ? JNI_TRUE : JNI_FALSE;
        }
        catch (const std::exception &e)
        {
            throwJavaException(env, "java/lang/RuntimeException", e.what());
            return JNI_FALSE;
        }
    }

    JNIEXPORT void JNICALL Java_org_maplibre_kmp_native_AwtCanvasRenderer_nativeUpdateSize(
        JNIEnv *env, jclass, jlong ptr, jint width, jint height)
    {
        try
        {
            auto *renderer = fromJavaPointer<maplibre_jni::AwtCanvasRenderer>(ptr);
            renderer->updateSize(width, height);
        }
        catch (const std::exception &e)
        {
            throwJavaException(env, "java/lang/RuntimeException", e.what());
        }
    }

    JNIEXPORT jlong JNICALL Java_org_maplibre_kmp_native_AwtCanvasRenderer_nativeGetRendererFrontend(
        JNIEnv *env, jclass, jlong ptr)
    {
        // Return the same pointer since AwtCanvasRenderer IS a RendererFrontend
        return ptr;
    }

} // extern "C"
