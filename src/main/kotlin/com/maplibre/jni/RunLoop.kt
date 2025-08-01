package com.maplibre.jni

/**
 * RunLoop processes asynchronous events for MapLibre Native.
 * 
 * This is essential for:
 * - Processing network responses
 * - Handling file source callbacks
 * - Managing timer events
 * - Executing async tasks
 * 
 * Without a RunLoop, network requests will be sent but responses
 * will never be processed, causing remote styles and tiles to never load.
 * 
 * Usage:
 * 1. Create RunLoop before creating the Map (sets as current thread's scheduler)
 * 2. Call runOnce() periodically from your render loop to process events
 * 3. The RunLoop must be on the same thread as the Map
 */
class RunLoop : NativeObject(
    new = { nativeCreateDefault() },
    destroy = ::nativeDestroy
) {
    /**
     * Process pending events without blocking.
     * 
     * This should be called periodically from your render loop
     * to process async callbacks, network responses, etc.
     * 
     * This is non-blocking and will return immediately if there
     * are no events to process.
     */
    fun runOnce() {
        nativeRunOnce(nativePtr)
    }
    
    /**
     * Stop the run loop.
     * 
     * This is typically called when shutting down.
     */
    fun stop() {
        nativeStop(nativePtr)
    }
    
    private companion object {
        init {
            MapLibreNativeLoader.load()
        }
        
        @JvmStatic external fun nativeCreateDefault(): Long
        @JvmStatic external fun nativeDestroy(ptr: Long)
        @JvmStatic external fun nativeRunOnce(ptr: Long)
        @JvmStatic external fun nativeStop(ptr: Long)
    }
}