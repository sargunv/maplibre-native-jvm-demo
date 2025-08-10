package org.maplibre.kmp.native


/**
 * The MaplibreMap class manages the map state, style, and camera position.
 * It also manages the renderer lifecycle, creating it internally from the provided Canvas.
 */
class MaplibreMap(
    private val frontend: RendererFrontend,
    private val mapObserver: MapObserver,
    mapOptions: MapOptions,
    private val resourceOptions: ResourceOptions,
    private val clientOptions: ClientOptions,
) : NativeObject(
  new = {
    // Create native JniRendererFrontend and Map
    if (frontend is CanvasRendererFrontend) {
      // bind back-pointer used for rendering
      frontend.jniFrontendPtr = org.maplibre.kmp.native.internal.Native.createJniRendererFrontend(frontend)
      // now that both renderer and frontend exist, bind observer
      if (frontend.rendererPtr != 0L) {
        org.maplibre.kmp.native.internal.Native.rendererBindObserver(frontend.rendererPtr, frontend.jniFrontendPtr)
      }
    }
    org.maplibre.kmp.native.internal.Native.nativeNewWithFrontend(
      (frontend as? CanvasRendererFrontend)?.jniFrontendPtr ?: org.maplibre.kmp.native.internal.Native.createJniRendererFrontend(frontend),
      mapObserver,
      mapOptions,
      resourceOptions,
      clientOptions
    )
  },
  destroy = { ptr ->
    org.maplibre.kmp.native.internal.Native.nativeDestroyWithFrontend(ptr)
    if (frontend is CanvasRendererFrontend && frontend.jniFrontendPtr != 0L) {
      org.maplibre.kmp.native.internal.Native.destroyJniRendererFrontend(frontend.jniFrontendPtr)
      frontend.jniFrontendPtr = 0
    }
  }
) {

  // Size changes should be handled by the frontend; MaplibreMap no longer listens to AWT events.

   /**
     * Process events and render if needed.
     * This should be called periodically from a timer (e.g., Swing Timer).
     * @return true if rendering occurred, false if there was nothing to render
     */
    fun tick(): Boolean {
        return (frontend as? CanvasRendererFrontend)?.tick() ?: false
    }
    

    /**
     * Triggers a repaint of the map.
     */
    fun triggerRepaint() {
        nativeTriggerRepaint(nativePtr)
    }

    /**
     * Loads a style from the given URL.
     * @param url The URL of the style to load (can be a local file:// URL)
     */
    fun loadStyleURL(url: String) {
        nativeLoadStyleURL(nativePtr, url)
    }

    /**
     * Loads a style from a JSON string.
     * @param json The style JSON as a string
     */
    fun loadStyleJSON(json: String) {
        nativeLoadStyleJSON(nativePtr, json)
    }

    /**
     * Updates the camera position.
     * @param options The camera options to apply
     */
    fun jumpTo(options: CameraOptions) {
        nativeJumpTo(nativePtr, options)
    }

    /**
     * Animates the camera to a new position.
     * @param options The camera options to animate to
     * @param duration The duration of the animation in milliseconds
     */
    fun easeTo(options: CameraOptions, duration: Int = 300) {
        nativeEaseTo(nativePtr, options, duration)
    }

    /**
     * Animates the camera to a new position using a flying motion.
     * @param options The camera options to fly to
     * @param duration The duration of the animation in milliseconds
     */
    fun flyTo(options: CameraOptions, duration: Int = 1000) {
        nativeFlyTo(nativePtr, options, duration)
    }

    /**
     * Gets the current camera options.
     * @return The current camera position and orientation
     */
    fun getCameraOptions(): CameraOptions {
        return nativeGetCameraOptions(nativePtr)
    }

    /**
     * Sets the map size. This should be called when the viewport resizes.
     * @param size The new size in pixels
     */
    fun setSize(size: Size) {
        nativeSetSize(nativePtr, size)
    }

    /**
     * Moves the map by the given screen coordinate offset.
     * @param screenCoordinate The offset to move by in pixels
     */
    fun moveBy(screenCoordinate: ScreenCoordinate) {
        nativeMoveBy(nativePtr, screenCoordinate)
    }

    /**
     * Scales the map by the given factor.
     * @param scale The scale factor (2.0 = zoom in, 0.5 = zoom out)
     * @param anchor Optional anchor point for the scale operation
     */
    fun scaleBy(scale: Double, anchor: ScreenCoordinate? = null) {
        nativeScaleBy(nativePtr, scale, anchor)
    }

    /**
     * Rotates the map from the first screen coordinate to the second.
     * @param first The starting point of the rotation gesture
     * @param second The ending point of the rotation gesture
     */
    fun rotateBy(first: ScreenCoordinate, second: ScreenCoordinate) {
        nativeRotateBy(nativePtr, first, second)
    }

    /**
     * Changes the map pitch by the given amount.
     * @param pitch The pitch change in degrees
     */
    fun pitchBy(pitch: Double) {
        nativePitchBy(nativePtr, pitch)
    }

    /**
     * Sets whether a gesture is currently in progress.
     * This helps the map optimize rendering during user interaction.
     * @param inProgress True if a gesture is in progress
     */
    fun setGestureInProgress(inProgress: Boolean) {
        nativeSetGestureInProgress(nativePtr, inProgress)
    }

    /**
     * Converts a geographic coordinate to a screen coordinate.
     * @param latLng The geographic coordinate
     * @return The screen coordinate in pixels
     */
    fun pixelForLatLng(latLng: LatLng): ScreenCoordinate {
        return nativePixelForLatLng(nativePtr, latLng)
    }

    /**
     * Converts a screen coordinate to a geographic coordinate.
     * @param pixel The screen coordinate in pixels
     * @return The geographic coordinate
     */
    fun latLngForPixel(pixel: ScreenCoordinate): LatLng {
        return nativeLatLngForPixel(nativePtr, pixel)
    }

    /**
     * Sets the debug options for the map.
     * @param options The debug options to enable
     */
    fun setDebug(options: MapDebugOptions) {
        nativeSetDebug(nativePtr, options.value)
    }
    
    /**
     * Gets the current debug options.
     * @return The current debug options
     */
    fun getDebug(): MapDebugOptions {
        return MapDebugOptions(nativeGetDebug(nativePtr))
    }
    
    /**
     * Enables or disables the rendering statistics view.
     * @param enabled True to show rendering stats, false to hide
     */
    fun enableRenderingStatsView(enabled: Boolean) {
        nativeEnableRenderingStatsView(nativePtr, enabled)
    }
    
    /**
     * Checks if the rendering statistics view is enabled.
     * @return True if rendering stats are shown
     */
    fun isRenderingStatsViewEnabled(): Boolean {
        return nativeIsRenderingStatsViewEnabled(nativePtr)
    }

    companion object {

        @JvmStatic
        private external fun nativeTriggerRepaint(ptr: Long)

        @JvmStatic
        private external fun nativeLoadStyleURL(ptr: Long, url: String)

        @JvmStatic
        private external fun nativeLoadStyleJSON(ptr: Long, json: String)

        @JvmStatic
        private external fun nativeJumpTo(ptr: Long, cameraOptions: CameraOptions)

        @JvmStatic
        private external fun nativeEaseTo(ptr: Long, cameraOptions: CameraOptions, duration: Int)

        @JvmStatic
        private external fun nativeFlyTo(ptr: Long, cameraOptions: CameraOptions, duration: Int)

        @JvmStatic
        private external fun nativeGetCameraOptions(ptr: Long): CameraOptions

        @JvmStatic
        private external fun nativeSetSize(ptr: Long, size: Size)


        @JvmStatic
        private external fun nativeMoveBy(ptr: Long, screenCoordinate: ScreenCoordinate)

        @JvmStatic
        private external fun nativeScaleBy(ptr: Long, scale: Double, anchor: ScreenCoordinate?)

        @JvmStatic
        private external fun nativeRotateBy(ptr: Long, first: ScreenCoordinate, second: ScreenCoordinate)

        @JvmStatic
        private external fun nativePitchBy(ptr: Long, pitch: Double)

        @JvmStatic
        private external fun nativeSetGestureInProgress(ptr: Long, inProgress: Boolean)

        @JvmStatic
        private external fun nativePixelForLatLng(ptr: Long, latLng: LatLng): ScreenCoordinate

        @JvmStatic
        private external fun nativeLatLngForPixel(ptr: Long, pixel: ScreenCoordinate): LatLng
        
        @JvmStatic
        private external fun nativeSetDebug(ptr: Long, debugOptions: Int)
        
        @JvmStatic
        private external fun nativeGetDebug(ptr: Long): Int
        
        @JvmStatic
        private external fun nativeEnableRenderingStatsView(ptr: Long, enabled: Boolean)
        
        @JvmStatic
        private external fun nativeIsRenderingStatsViewEnabled(ptr: Long): Boolean
    }
}
