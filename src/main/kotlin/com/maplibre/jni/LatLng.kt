package com.maplibre.jni

class LatLng internal constructor(
    private val createNative: () -> Long
) : NativeObject(
    new = createNative,
    destroy = ::nativeDestroy
) {
    @JvmOverloads
    constructor(
        latitude: Double,
        longitude: Double,
        wrap: Boolean = false
    ) : this({
        require(!latitude.isNaN()) { "latitude must not be NaN" }
        require(!longitude.isNaN()) { "longitude must not be NaN" }
        require(latitude in -90.0..90.0) { "latitude must be between -90 and 90" }
        require(longitude.isFinite()) { "longitude must not be infinite" }
        nativeNew(latitude, longitude, wrap)
    })
    
    val latitude: Double get() = nativeLatitude(nativePtr)
    val longitude: Double get() = nativeLongitude(nativePtr)
    
    fun wrapped(): LatLng = LatLng { nativeWrapped(nativePtr) }
    
    fun wrap() {
        nativeWrap(nativePtr)
    }
    
    fun unwrapForShortestPath(end: LatLng) {
        nativeUnwrapForShortestPath(nativePtr, end.nativePtr)
    }
    
    override fun equals(other: Any?): Boolean {
        if (this === other) return true
        if (other !is LatLng) return false
        return nativeEquals(nativePtr, other.nativePtr)
    }
    
    override fun hashCode(): Int {
        return 31 * latitude.hashCode() + longitude.hashCode()
    }
    
    override fun toString(): String {
        return "LatLng(latitude=$latitude, longitude=$longitude)"
    }
    
    private companion object {
        init {
            MapLibreNativeLoader.load()
        }
        
        @JvmStatic external fun nativeNew(latitude: Double, longitude: Double, wrap: Boolean): Long
        @JvmStatic external fun nativeLatitude(ptr: Long): Double
        @JvmStatic external fun nativeLongitude(ptr: Long): Double
        @JvmStatic external fun nativeWrapped(ptr: Long): Long
        @JvmStatic external fun nativeWrap(ptr: Long)
        @JvmStatic external fun nativeUnwrapForShortestPath(ptr: Long, endPtr: Long)
        @JvmStatic external fun nativeEquals(ptr: Long, otherPtr: Long): Boolean
        @JvmStatic external fun nativeDestroy(ptr: Long)
    }
}
