package com.maplibre.jni

import java.awt.Canvas
import java.awt.event.ComponentAdapter
import java.awt.event.ComponentEvent

/**
 * The MaplibreMap class manages the map state, style, and camera position.
 * It also manages the renderer lifecycle, creating it internally from the provided Canvas.
 */
class MaplibreMap(
    private val canvas: Canvas,  // Keep reference to prevent GC
    private val mapObserver: MapObserver,  // Keep reference to prevent GC
    mapOptions: MapOptions,
    private val resourceOptions: ResourceOptions,
    private val clientOptions: ClientOptions,
) : NativeObject(
  new = {
    val pixelRatio = canvas.graphicsConfiguration?.defaultTransform?.scaleX?.toFloat()
      ?: 1.0f
    nativeNew(
      canvas = canvas,
      width = (canvas.width * pixelRatio).toInt(),
      height = (canvas.height * pixelRatio).toInt(),
      pixelRatio = pixelRatio,
      mapObserver = mapObserver,
      mapOptions = mapOptions,
      resourceOptions = resourceOptions,
      clientOptions = clientOptions
    )
  },
  destroy = ::nativeDestroy
) {

  init {
    canvas.addComponentListener(object : ComponentAdapter() {
      override fun componentResized(e: ComponentEvent) {
        val scale =
          canvas.graphicsConfiguration!!.defaultTransform.scaleX.toFloat()
        val pixelWidth = (canvas.width * scale).toInt()
        val pixelHeight = (canvas.height * scale).toInt()
        this@MaplibreMap.setSize(Size(pixelWidth, pixelHeight))
      }
    })
  }

   /**
     * Process events and render if needed.
     * This should be called periodically from a timer (e.g., Swing Timer).
     * @return true if rendering occurred, false if there was nothing to render
     */
    fun tick(): Boolean {
        return nativeTick(nativePtr)
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
    internal fun setSize(size: Size) {
        nativeSetSize(nativePtr, size)
    }

    companion object {

        @JvmStatic
        private external fun nativeNew(
            canvas: Canvas,
            width: Int,
            height: Int,
            pixelRatio: Float,
            mapObserver: MapObserver,
            mapOptions: MapOptions,
            resourceOptions: ResourceOptions,
            clientOptions: ClientOptions
        ): Long

        @JvmStatic
        private external fun nativeDestroy(ptr: Long)

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
        private external fun nativeTick(ptr: Long): Boolean
    }
}
