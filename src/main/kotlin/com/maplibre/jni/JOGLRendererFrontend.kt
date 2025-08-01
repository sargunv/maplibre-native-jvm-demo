package com.maplibre.jni

/**
 * JOGL-based renderer frontend for MapLibre Native.
 * This implementation integrates MapLibre's rendering with JOGL's OpenGL context.
 */
class JOGLRendererFrontend(
    backend: JOGLRendererBackend,
    pixelRatio: Float
) : NativeObject(
    new = { nativeNew(backend.nativePtr, pixelRatio) },
    destroy = ::nativeDestroy
) {
    /**
     * Render the current frame.
     * This should be called from JOGL's display callback.
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