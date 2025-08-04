package com.maplibre.jni

import java.awt.Canvas

/**
 * Unified renderer for MapLibre that handles both frontend and backend responsibilities.
 * This renderer manages the complete rendering pipeline for displaying maps in AWT Canvas.
 * 
 * The renderer internally manages:
 * - Platform-specific graphics backend (Metal on macOS, OpenGL ES on Linux)
 * - Render state and dirty tracking
 * - Event processing via RunLoop
 * - Buffer swapping and presentation
 */
class AwtCanvasRenderer(
    canvas: Canvas,
    width: Int,
    height: Int,
    pixelRatio: Float
) : NativeObject(
    new = { nativeCreate(canvas, width, height, pixelRatio) },
    destroy = { ptr -> nativeDestroy(ptr) }
) {
    
    /**
     * Process events and render if needed.
     * This should be called periodically from a timer (e.g., Swing Timer).
     * 
     * @return true if rendering occurred, false if there was nothing to render
     */
    fun tick(): Boolean {
        return nativeTick(nativePtr)
    }
    
    /**
     * Update the size of the rendering surface.
     * This should be called when the canvas is resized.
     * 
     * @param width The new width in pixels
     * @param height The new height in pixels
     */
    fun updateSize(width: Int, height: Int) {
        nativeUpdateSize(nativePtr, width, height)
    }
    
    /**
     * Get the native pointer to the RendererFrontend.
     * This is used internally by MaplibreMap.
     */
    internal fun getRendererFrontend(): Long {
        return nativeGetRendererFrontend(nativePtr)
    }
    
    companion object {
        init {
            MapLibreNativeLoader.load()
        }
        
        @JvmStatic
        private external fun nativeCreate(canvas: Canvas, width: Int, height: Int, pixelRatio: Float): Long
        
        @JvmStatic
        private external fun nativeDestroy(ptr: Long)
        
        @JvmStatic
        private external fun nativeTick(ptr: Long): Boolean
        
        @JvmStatic
        private external fun nativeUpdateSize(ptr: Long, width: Int, height: Int)
        
        @JvmStatic
        private external fun nativeGetRendererFrontend(ptr: Long): Long
    }
}