package org.maplibre.kmp.native

/**
 * Interface for observing map events. All methods have default empty implementations
 * so you only need to override the ones you're interested in.
 */
interface MapObserver {
    enum class CameraChangeMode {
        IMMEDIATE,
        ANIMATED
    }

    enum class RenderMode {
        PARTIAL,
        FULL
    }

    data class RenderFrameStatus(
        val mode: RenderMode,
        val needsRepaint: Boolean,
        val placementChanged: Boolean,
        // TODO: Add renderingStats when we wrap that type
    )

    // Camera events
    fun onCameraWillChange(mode: CameraChangeMode) {}
    fun onCameraIsChanging() {}
    fun onCameraDidChange(mode: CameraChangeMode) {}

    // Map loading events
    fun onWillStartLoadingMap() {}
    fun onDidFinishLoadingMap() {}
    fun onDidFailLoadingMap(error: MapLoadError, message: String) {}

    // Rendering events - most important for MVP
    fun onWillStartRenderingFrame() {}
    fun onDidFinishRenderingFrame(status: RenderFrameStatus) {}
    fun onWillStartRenderingMap() {}
    fun onDidFinishRenderingMap(mode: RenderMode) {}

    // Style events
    fun onDidFinishLoadingStyle() {}
    fun onStyleImageMissing(imageId: String) {}

    // Idle state
    fun onDidBecomeIdle() {}

    // TODO: Add these callbacks when we have the necessary types:
    // - onSourceChanged(source: Source)
    // - onCanRemoveUnusedStyleImage(imageId: String): Boolean
    // - onRegisterShaders(registry: ShaderRegistry)
    // - Shader compilation callbacks
    // - Glyph loading callbacks
    // - Tile loading callbacks
    // - Sprite loading callbacks
}
