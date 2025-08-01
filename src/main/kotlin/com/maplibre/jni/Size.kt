package com.maplibre.jni

class Size internal constructor(
    private val createNative: () -> Long
) : NativeObject(
    new = createNative,
    destroy = ::nativeDestroy
) {
    constructor(width: Int, height: Int) : this({ nativeNew(width, height) })
    
    val width: Int get() = nativeWidth(nativePtr)
    val height: Int get() = nativeHeight(nativePtr)
    val area: Int get() = nativeArea(nativePtr)
    val aspectRatio: Float get() = nativeAspectRatio(nativePtr)
    val isEmpty: Boolean get() = nativeIsEmpty(nativePtr)
    
    override fun toString(): String = "Size($width, $height)"
    
    override fun equals(other: Any?): Boolean {
        if (this === other) return true
        if (other !is Size) return false
        return nativeEquals(nativePtr, other.nativePtr)
    }
    
    override fun hashCode(): Int {
        return 31 * width.hashCode() + height.hashCode()
    }
    
    private companion object {
        init {
            MapLibreNativeLoader.load()
        }

        @JvmStatic external fun nativeNew(width: Int, height: Int): Long
        @JvmStatic external fun nativeWidth(ptr: Long): Int
        @JvmStatic external fun nativeHeight(ptr: Long): Int
        @JvmStatic external fun nativeArea(ptr: Long): Int
        @JvmStatic external fun nativeAspectRatio(ptr: Long): Float
        @JvmStatic external fun nativeIsEmpty(ptr: Long): Boolean
        @JvmStatic external fun nativeEquals(ptr: Long, otherPtr: Long): Boolean
        @JvmStatic external fun nativeDestroy(ptr: Long)
    }
}
