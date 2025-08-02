package com.maplibre.jni

import java.awt.Canvas

/**
 * EGL-based renderer backend that works on all platforms.
 * Uses ANGLE on macOS/Windows for Metal/D3D11 support,
 * and native EGL on Linux.
 */
class EGLRendererBackend(
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
    
    companion object {
        @JvmStatic
        private external fun nativeCreate(canvas: Canvas, width: Int, height: Int): Long
        
        @JvmStatic
        private external fun nativeDestroy(ptr: Long)
        
        @JvmStatic
        private external fun nativeUpdateSize(ptr: Long, width: Int, height: Int)
        
        @JvmStatic
        private external fun nativeSwap(ptr: Long)
    }
}