#pragma once

#include <jni.h>
#include <mbgl/renderer/renderer_frontend.hpp>
#include <mbgl/renderer/renderer_observer.hpp>
#include <mbgl/actor/scheduler.hpp>
#include <mbgl/util/run_loop.hpp>
#include <memory>
#include <atomic>

// Forward declarations
namespace mbgl {
class Renderer;
class UpdateParameters;
namespace gfx {
class RendererBackend;
}
}

namespace maplibre_jni {

// Unified renderer that combines frontend and backend functionality for AWT Canvas
// This class manages the complete rendering pipeline for MapLibre in Java AWT
class AwtCanvasRenderer : public mbgl::RendererFrontend {
public:
    // Create a renderer for the given AWT Canvas
    static std::unique_ptr<AwtCanvasRenderer> create(
        JNIEnv* env,
        jobject canvas,
        int width,
        int height,
        const std::optional<std::string>& localFontFamily = std::nullopt
    );
    
    ~AwtCanvasRenderer() override;
    
    // Process events and render if needed
    // Returns true if rendering occurred, false otherwise
    bool tick();
    
    // Update the size of the rendering surface
    void updateSize(int width, int height);
    
    // RendererFrontend implementation
    void reset() override;
    void setObserver(mbgl::RendererObserver& observer) override;
    void update(std::shared_ptr<mbgl::UpdateParameters> parameters) override;
    const mbgl::TaggedScheduler& getThreadPool() const override;
    
    // Get the underlying renderer backend for Map creation
    mbgl::gfx::RendererBackend* getRendererBackend();
    
protected:
    // Hide constructor to force use of factory method
    AwtCanvasRenderer();
    
private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace maplibre_jni