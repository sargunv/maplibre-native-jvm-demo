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

// JOGL-specific renderer frontend that encapsulates the complexity of MapLibre rendering
// This follows the pattern from maplibre-native-rs to avoid missing header dependencies
class JOGLRendererFrontend : public mbgl::RendererFrontend {
public:
    // Create a frontend with the given backend and pixel ratio
    static std::unique_ptr<JOGLRendererFrontend> create(
        mbgl::gfx::RendererBackend& backend,
        float pixelRatio,
        const std::optional<std::string>& localFontFamily = std::nullopt
    );
    
    ~JOGLRendererFrontend() override;
    
    // RendererFrontend implementation
    void reset() override;
    void setObserver(mbgl::RendererObserver& observer) override;
    void update(std::shared_ptr<mbgl::UpdateParameters> parameters) override;
    const mbgl::TaggedScheduler& getThreadPool() const override;
    
    // Additional methods for JOGL integration
    void render();
    
protected:
    // Hide constructor to force use of factory method
    JOGLRendererFrontend();
    
private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

}