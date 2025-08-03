package com.maplibre.jni

import java.awt.Canvas

/**
 * Platform-specific renderer backend using JAWT for native window access.
 * Uses Metal on macOS and OpenGL ES (via EGL) on Linux/Windows.
 */
class JAWTRendererBackend(
    canvas: Canvas,
    width: Int,
    height: Int
) : NativeObject(
    new = { nativeCreate(canvas, width, height) },
    destroy = { ptr -> nativeDestroy(ptr) }
) {
    
    fun updateSize(width: Int, height: Int) {
        nativeUpdateSize(nativePtr, width, height)
    }
    
    fun swap() {
        nativeSwap(nativePtr)
    }
    
    internal fun getRendererBackend(): Long {
        return nativeGetRendererBackend(nativePtr)
    }
    
    companion object {
        @JvmStatic
        private external fun nativeCreate(canvas: Canvas, width: Int, height: Int): Long
        
        @JvmStatic
        private external fun nativeDestroy(ptr: Long)
        
        @JvmStatic
        private external fun nativeUpdateSize(ptr: Long, width: Int, height: Int)
        
        @JvmStatic
        private external fun nativeSwap(ptr: Long)
        
        @JvmStatic
        private external fun nativeGetRendererBackend(ptr: Long): Long
    }
}