package com.maplibre.jni

class EdgeInsets internal constructor(
    private val createNative: () -> Long
) : NativeObject(
    new = createNative,
    destroy = ::nativeDestroy
) {
    @JvmOverloads
    constructor(
        top: Double = 0.0,
        left: Double = 0.0,
        bottom: Double = 0.0,
        right: Double = 0.0
    ) : this({
        require(!top.isNaN()) { "top must not be NaN" }
        require(!left.isNaN()) { "left must not be NaN" }
        require(!bottom.isNaN()) { "bottom must not be NaN" }
        require(!right.isNaN()) { "right must not be NaN" }
        nativeNew(top, left, bottom, right)
    })
    
    val top: Double get() = nativeTop(nativePtr)
    val left: Double get() = nativeLeft(nativePtr)
    val bottom: Double get() = nativeBottom(nativePtr)
    val right: Double get() = nativeRight(nativePtr)
    val isFlush: Boolean get() = nativeIsFlush(nativePtr)
    
    operator fun plus(other: EdgeInsets): EdgeInsets = EdgeInsets(
        top + other.top,
        left + other.left,
        bottom + other.bottom,
        right + other.right
    )
    
    operator fun plusAssign(other: EdgeInsets) {
        nativePlusAssign(nativePtr, other.nativePtr)
    }
    
    fun getCenter(width: Int, height: Int): ScreenCoordinate {
        return ScreenCoordinate { nativeGetCenter(nativePtr, width, height) }
    }
    
    override fun equals(other: Any?): Boolean {
        if (this === other) return true
        if (other !is EdgeInsets) return false
        return nativeEquals(nativePtr, other.nativePtr)
    }
    
    override fun hashCode(): Int {
        var result = top.hashCode()
        result = 31 * result + left.hashCode()
        result = 31 * result + bottom.hashCode()
        result = 31 * result + right.hashCode()
        return result
    }
    
    override fun toString(): String = "EdgeInsets(top=$top, left=$left, bottom=$bottom, right=$right)"
    
    private companion object {
        init {
            MapLibreNativeLoader.load()
        }
        
        @JvmStatic external fun nativeNew(top: Double, left: Double, bottom: Double, right: Double): Long
        @JvmStatic external fun nativeTop(ptr: Long): Double
        @JvmStatic external fun nativeLeft(ptr: Long): Double
        @JvmStatic external fun nativeBottom(ptr: Long): Double
        @JvmStatic external fun nativeRight(ptr: Long): Double
        @JvmStatic external fun nativeIsFlush(ptr: Long): Boolean
        @JvmStatic external fun nativePlusAssign(ptr: Long, otherPtr: Long)
        @JvmStatic external fun nativeGetCenter(ptr: Long, width: Int, height: Int): Long
        @JvmStatic external fun nativeEquals(ptr: Long, otherPtr: Long): Boolean
        @JvmStatic external fun nativeDestroy(ptr: Long)
    }
}