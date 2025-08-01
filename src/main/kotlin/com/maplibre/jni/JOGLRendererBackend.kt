package com.maplibre.jni

/**
 * JOGL-based renderer backend for MapLibre Native.
 * This provides the OpenGL context integration needed by MapLibre.
 */
class JOGLRendererBackend(
    glContext: GLContext,
    width: Int,
    height: Int
) : NativeObject(
    new = { nativeNew(glContext, width, height) },
    destroy = ::nativeDestroy
) {
    /**
     * Updates the size of the rendering surface.
     * Should be called when the window or panel is resized.
     */
    fun updateSize(width: Int, height: Int) {
        nativeUpdateSize(nativePtr, width, height)
    }
    
    companion object {
        init {
            MapLibreNativeLoader.load()
        }
        
        @JvmStatic
        private external fun nativeNew(glContext: GLContext, width: Int, height: Int): Long
        
        @JvmStatic
        private external fun nativeDestroy(ptr: Long)
        
        @JvmStatic
        private external fun nativeUpdateSize(ptr: Long, width: Int, height: Int)
    }
}