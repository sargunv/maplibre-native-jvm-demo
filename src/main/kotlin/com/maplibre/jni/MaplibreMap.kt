package com.maplibre.jni

import java.lang.ref.Cleaner

/**
 * The MaplibreMap class manages the map state, style, and camera position.
 * It connects the renderer frontend with map data and provides the main API
 * for interacting with the map.
 */
class MaplibreMap(
    rendererFrontend: RendererFrontend,
    mapObserver: MapObserver,
    mapOptions: MapOptions,
    resourceOptions: ResourceOptions,
    clientOptions: ClientOptions = ClientOptions()
) : AutoCloseable {
    
    // Keep a reference to prevent garbage collection while the map exists
    private val nativeObserver: NativeMapObserver = NativeMapObserver(mapObserver)
    
    // Create the map with all the components
    private val nativePtr: Long = nativeNew(
        rendererFrontend.nativePtr,
        nativeObserver.nativePtr,
        mapOptions.nativePtr,
        resourceOptions.nativePtr,
        clientOptions.nativePtr
    )
    
    // Store resource options for file source activation (will be called after construction)
    private val storedResourceOptions = resourceOptions
    private val storedClientOptions = clientOptions
    
    // Register cleanup
    private val cleanable = cleaner.register(this) {
        nativeDestroy(nativePtr)
    }
    
    override fun close() {
        cleanable.clean()
        nativeObserver.close()
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
        nativeJumpTo(nativePtr, options.nativePtr)
    }
    
    /**
     * Animates the camera to a new position.
     * @param options The camera options to animate to
     * @param duration The duration of the animation in milliseconds
     */
    fun easeTo(options: CameraOptions, duration: Int = 300) {
        nativeEaseTo(nativePtr, options.nativePtr, duration)
    }
    
    /**
     * Animates the camera to a new position using a flying motion.
     * @param options The camera options to fly to
     * @param duration The duration of the animation in milliseconds
     */
    fun flyTo(options: CameraOptions, duration: Int = 1000) {
        nativeFlyTo(nativePtr, options.nativePtr, duration)
    }
    
    /**
     * Gets the current camera options.
     * @return The current camera position and orientation
     */
    fun getCameraOptions(): CameraOptions {
        val ptr = nativeGetCameraOptions(nativePtr)
        return CameraOptions(createNative = { ptr })
    }
    
    /**
     * Sets the map size. This should be called when the viewport resizes.
     * @param size The new size in pixels
     */
    fun setSize(size: Size) {
        nativeSetSize(nativePtr, size.nativePtr)
    }
    
    /**
     * Activates file sources for network and resource loading.
     * This must be called before loading remote styles to enable network requests.
     */
    fun activateFileSources() {
        nativeActivateFileSources(storedResourceOptions.nativePtr, storedClientOptions.nativePtr)
    }
    
    companion object {
        private val cleaner = Cleaner.create()
        
        @JvmStatic
        private external fun nativeNew(
            rendererFrontendPtr: Long,
            mapObserverPtr: Long,
            mapOptionsPtr: Long,
            resourceOptionsPtr: Long,
            clientOptionsPtr: Long
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
        private external fun nativeJumpTo(ptr: Long, cameraOptionsPtr: Long)
        
        @JvmStatic
        private external fun nativeEaseTo(ptr: Long, cameraOptionsPtr: Long, duration: Int)
        
        @JvmStatic
        private external fun nativeFlyTo(ptr: Long, cameraOptionsPtr: Long, duration: Int)
        
        @JvmStatic
        private external fun nativeGetCameraOptions(ptr: Long): Long
        
        @JvmStatic
        private external fun nativeSetSize(ptr: Long, sizePtr: Long)
        
        @JvmStatic
        private external fun nativeActivateFileSources(resourceOptionsPtr: Long, clientOptionsPtr: Long)
    }
}