package com.maplibre.jni

/**
 * Renderer frontend for MapLibre Native.
 * This implementation works with any renderer backend (Metal, OpenGL ES, etc.)
 */
class RendererFrontend(
    backendPtr: Long,  // Accept raw pointer to any backend
    pixelRatio: Float,
    updateCallback: (() -> Unit)? = null
) : NativeObject(
    new = { nativeNew(backendPtr, pixelRatio) },
    destroy = ::nativeDestroy
) {
    init {
        // Set the update callback if provided
        updateCallback?.let { setUpdateCallback(it) }
    }
    
    /**
     * Render the current frame.
     * This should be called from the render loop.
     */
    fun render() {
        nativeRender(nativePtr)
    }
    
    /**
     * Set a callback that will be invoked when the frontend needs to update.
     * This happens when tiles load, styles change, etc.
     */
    fun setUpdateCallback(callback: () -> Unit) {
        // Wrap the Kotlin lambda in a Runnable for JNI compatibility
        nativeSetUpdateCallback(nativePtr, Runnable { callback() })
    }
    
    companion object {
        init {
            MapLibreNativeLoader.load()
        }
        
        @JvmStatic
        private external fun nativeNew(backendPtr: Long, pixelRatio: Float): Long
        
        @JvmStatic
        private external fun nativeDestroy(ptr: Long)
        
        @JvmStatic
        private external fun nativeRender(ptr: Long)
        
        @JvmStatic
        private external fun nativeSetUpdateCallback(ptr: Long, callback: Runnable)
    }
}