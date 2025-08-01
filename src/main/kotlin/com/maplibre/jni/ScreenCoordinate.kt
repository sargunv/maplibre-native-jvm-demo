package com.maplibre.jni

class ScreenCoordinate internal constructor(
    private val createNative: () -> Long
) : NativeObject(
    new = createNative,
    destroy = ::nativeDestroy
) {
    constructor(x: Double, y: Double) : this({ nativeNew(x, y) })
    
    val x: Double get() = nativeX(nativePtr)
    val y: Double get() = nativeY(nativePtr)
    
    override fun equals(other: Any?): Boolean {
        if (this === other) return true
        if (other !is ScreenCoordinate) return false
        return nativeEquals(nativePtr, other.nativePtr)
    }
    
    override fun hashCode(): Int = 31 * x.hashCode() + y.hashCode()
    
    override fun toString(): String = "ScreenCoordinate($x, $y)"
    
    private companion object {
        init {
            MapLibreNativeLoader.load()
        }
        
        @JvmStatic external fun nativeNew(x: Double, y: Double): Long
        @JvmStatic external fun nativeX(ptr: Long): Double
        @JvmStatic external fun nativeY(ptr: Long): Double
        @JvmStatic external fun nativeEquals(ptr: Long, otherPtr: Long): Boolean
        @JvmStatic external fun nativeDestroy(ptr: Long)
    }
}
