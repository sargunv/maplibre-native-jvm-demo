#pragma once

#include <mbgl/renderer/renderer_frontend.hpp>
#include <mbgl/renderer/renderer_observer.hpp>
#include <mbgl/actor/scheduler.hpp>
#include <memory>

// Forward declarations to avoid including problematic headers
namespace mbgl {
class Renderer;
class UpdateParameters;
namespace gfx {
class RendererBackend;
}
}

namespace maplibre_jni {

// Platform-agnostic renderer frontend that encapsulates the complexity of MapLibre rendering
// This follows the pattern from maplibre-native-rs to avoid missing header dependencies
class RendererFrontend : public mbgl::RendererFrontend {
public:
    // Callback type for update notifications
    using UpdateCallback = std::function<void()>;
    
    // Create a frontend with the given backend and pixel ratio
    static std::unique_ptr<RendererFrontend> create(
        mbgl::gfx::RendererBackend& backend,
        float pixelRatio,
        const std::optional<std::string>& localFontFamily = std::nullopt
    );
    
    ~RendererFrontend() override;
    
    // RendererFrontend implementation
    void reset() override;
    void setObserver(mbgl::RendererObserver& observer) override;
    void update(std::shared_ptr<mbgl::UpdateParameters> parameters) override;
    const mbgl::TaggedScheduler& getThreadPool() const override;
    
    // Additional methods for rendering
    void render();
    
    // Set callback to be invoked when update() is called
    void setUpdateCallback(UpdateCallback callback);
    
protected:
    // Hide constructor to force use of factory method
    RendererFrontend();
    
private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

}