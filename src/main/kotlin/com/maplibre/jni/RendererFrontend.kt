package com.maplibre.jni

/**
 * Renderer frontend for MapLibre Native.
 * This implementation works with any renderer backend (EGL, JOGL, etc.)
 */
class RendererFrontend(
    backendPtr: Long,  // Accept raw pointer to any backend
    pixelRatio: Float
) : NativeObject(
    new = { nativeNew(backendPtr, pixelRatio) },
    destroy = ::nativeDestroy
) {
    /**
     * Render the current frame.
     * This should be called from the render loop.
     */
    fun render() {
        nativeRender(nativePtr)
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
    }
}