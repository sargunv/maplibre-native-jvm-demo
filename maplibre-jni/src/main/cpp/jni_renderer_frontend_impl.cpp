#include "jni_renderer_frontend.hpp"
#include <mbgl/actor/scheduler.hpp>
#include <mbgl/util/util.hpp>
#include <mbgl/renderer/renderer.hpp>
#include <mbgl/renderer/update_parameters.hpp>
#include <mbgl/gfx/backend_scope.hpp>

namespace maplibre_jni {

class RendererFrontend::Impl {
public:
    Impl(mbgl::gfx::RendererBackend& backend, float pixelRatio, const std::optional<std::string>& localFontFamily)
        : backend(backend),
          scheduler(mbgl::Scheduler::GetCurrent()),
          threadPool(scheduler, mbgl::util::SimpleIdentity()) {
        renderer = std::make_unique<mbgl::Renderer>(backend, pixelRatio, localFontFamily);
    }
    
    ~Impl() {
        reset();
    }
    
    void reset() {
        if (observer && renderer) {
            renderer->setObserver(nullptr);
            observer = nullptr;
        }
        if (renderer) {
            renderer.reset();
        }
    }
    
    void setObserver(mbgl::RendererObserver& obs) {
        observer = &obs;
        if (renderer) {
            renderer->setObserver(observer);
        }
    }
    
    void update(std::shared_ptr<mbgl::UpdateParameters> parameters) {
        updateParameters = parameters;
        if (updateCallback) {
            updateCallback();
        }
    }
    
    void render() {
        if (!renderer) {
            throw std::runtime_error("Renderer not initialized");
        }
        
        if (!updateParameters) {
            return;
        }
        
        mbgl::gfx::BackendScope guard{backend, mbgl::gfx::BackendScope::ScopeType::Implicit};
        
        auto params = updateParameters;
        renderer->render(params);
    }
    
    const mbgl::TaggedScheduler& getThreadPool() const {
        return threadPool;
    }
    
    void setUpdateCallback(RendererFrontend::UpdateCallback callback) {
        updateCallback = callback;
    }
    
private:
    mbgl::gfx::RendererBackend& backend;
    std::unique_ptr<mbgl::Renderer> renderer;
    mbgl::RendererObserver* observer = nullptr;
    std::shared_ptr<mbgl::UpdateParameters> updateParameters;
    std::shared_ptr<mbgl::Scheduler> scheduler;
    mbgl::TaggedScheduler threadPool;
    RendererFrontend::UpdateCallback updateCallback;
};

std::unique_ptr<RendererFrontend> RendererFrontend::create(
    mbgl::gfx::RendererBackend& backend,
    float pixelRatio,
    const std::optional<std::string>& localFontFamily) {
    
    auto frontend = std::unique_ptr<RendererFrontend>(new RendererFrontend());
    frontend->impl = std::make_unique<Impl>(backend, pixelRatio, localFontFamily);
    return frontend;
}

RendererFrontend::RendererFrontend() = default;
RendererFrontend::~RendererFrontend() {
    if (impl) {
        impl.reset();
    }
}

void RendererFrontend::reset() {
    if (impl) {
        impl->reset();
    }
}

void RendererFrontend::setObserver(mbgl::RendererObserver& observer) {
    if (impl) {
        impl->setObserver(observer);
    }
}

void RendererFrontend::update(std::shared_ptr<mbgl::UpdateParameters> parameters) {
    if (impl) {
        impl->update(parameters);
    }
}

const mbgl::TaggedScheduler& RendererFrontend::getThreadPool() const {
    if (impl) {
        return impl->getThreadPool();
    }
    throw std::runtime_error("RendererFrontend not initialized");
}

void RendererFrontend::render() {
    if (impl) {
        impl->render();
    }
}

void RendererFrontend::setUpdateCallback(UpdateCallback callback) {
    if (impl) {
        impl->setUpdateCallback(callback);
    }
}

}