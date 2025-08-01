package com.maplibre.jni

/**
 * Wraps a native MapObserver instance. This is used internally by the Map
 * to connect a Kotlin MapObserver to the native map.
 */
internal class NativeMapObserver(
    private val kotlinObserver: MapObserver
) : NativeObject(
    new = { nativeNew(kotlinObserver) },
    destroy = ::nativeDestroy
) {
    
    private companion object {
        init {
            MapLibreNativeLoader.load()
        }
        
        @JvmStatic external fun nativeNew(observer: MapObserver): Long
        @JvmStatic external fun nativeDestroy(ptr: Long)
    }
}