package com.maplibre.jni

class MapOptions internal constructor(
    private val createNative: () -> Long = ::nativeNew
) : NativeObject(
    new = createNative,
    destroy = ::nativeDestroy
) {
    constructor() : this(::nativeNew)
    
    fun withMapMode(mode: MapMode): MapOptions {
        nativeSetMapMode(nativePtr, mode.nativeValue)
        return this
    }
    
    fun withConstrainMode(mode: ConstrainMode): MapOptions {
        nativeSetConstrainMode(nativePtr, mode.nativeValue)
        return this
    }
    
    fun withViewportMode(mode: ViewportMode): MapOptions {
        nativeSetViewportMode(nativePtr, mode.nativeValue)
        return this
    }
    
    fun withCrossSourceCollisions(enabled: Boolean): MapOptions {
        nativeSetCrossSourceCollisions(nativePtr, enabled)
        return this
    }
    
    fun withNorthOrientation(orientation: NorthOrientation): MapOptions {
        nativeSetNorthOrientation(nativePtr, orientation.nativeValue)
        return this
    }
    
    fun withSize(size: Size): MapOptions {
        nativeSetSize(nativePtr, size)
        return this
    }
    
    fun withPixelRatio(ratio: Float): MapOptions {
        require(ratio > 0) { "pixelRatio must be positive" }
        nativeSetPixelRatio(nativePtr, ratio)
        return this
    }
    
    val mapMode: MapMode get() = MapMode.fromNative(nativeGetMapMode(nativePtr))
    
    val constrainMode: ConstrainMode get() = ConstrainMode.fromNative(nativeGetConstrainMode(nativePtr))
    
    val viewportMode: ViewportMode get() = ViewportMode.fromNative(nativeGetViewportMode(nativePtr))
    
    val crossSourceCollisions: Boolean get() = nativeGetCrossSourceCollisions(nativePtr)
    
    val northOrientation: NorthOrientation get() = NorthOrientation.fromNative(nativeGetNorthOrientation(nativePtr))
    
    val size: Size get() = nativeGetSize(nativePtr)
    
    val pixelRatio: Float get() = nativeGetPixelRatio(nativePtr)
    
    override fun equals(other: Any?): Boolean {
        if (this === other) return true
        if (other !is MapOptions) return false
        return mapMode == other.mapMode &&
               constrainMode == other.constrainMode &&
               viewportMode == other.viewportMode &&
               crossSourceCollisions == other.crossSourceCollisions &&
               northOrientation == other.northOrientation &&
               size == other.size &&
               pixelRatio == other.pixelRatio
    }
    
    override fun hashCode(): Int {
        var result = mapMode.hashCode()
        result = 31 * result + constrainMode.hashCode()
        result = 31 * result + viewportMode.hashCode()
        result = 31 * result + crossSourceCollisions.hashCode()
        result = 31 * result + northOrientation.hashCode()
        result = 31 * result + size.hashCode()
        result = 31 * result + pixelRatio.hashCode()
        return result
    }
    
    override fun toString(): String {
        return "MapOptions(mapMode=$mapMode, constrainMode=$constrainMode, viewportMode=$viewportMode, " +
               "crossSourceCollisions=$crossSourceCollisions, northOrientation=$northOrientation, " +
               "size=$size, pixelRatio=$pixelRatio)"
    }
    
    private companion object {
        init {
            MapLibreNativeLoader.load()
        }
        
        @JvmStatic external fun nativeNew(): Long
        @JvmStatic external fun nativeDestroy(ptr: Long)
        @JvmStatic external fun nativeSetMapMode(ptr: Long, mode: Int)
        @JvmStatic external fun nativeSetConstrainMode(ptr: Long, mode: Int)
        @JvmStatic external fun nativeSetViewportMode(ptr: Long, mode: Int)
        @JvmStatic external fun nativeSetCrossSourceCollisions(ptr: Long, enabled: Boolean)
        @JvmStatic external fun nativeSetNorthOrientation(ptr: Long, orientation: Int)
        @JvmStatic external fun nativeSetSize(ptr: Long, size: Size)
        @JvmStatic external fun nativeSetPixelRatio(ptr: Long, ratio: Float)
        @JvmStatic external fun nativeGetMapMode(ptr: Long): Int
        @JvmStatic external fun nativeGetConstrainMode(ptr: Long): Int
        @JvmStatic external fun nativeGetViewportMode(ptr: Long): Int
        @JvmStatic external fun nativeGetCrossSourceCollisions(ptr: Long): Boolean
        @JvmStatic external fun nativeGetNorthOrientation(ptr: Long): Int
        @JvmStatic external fun nativeGetSize(ptr: Long): Size
        @JvmStatic external fun nativeGetPixelRatio(ptr: Long): Float
    }
}