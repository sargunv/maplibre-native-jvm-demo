package com.maplibre.jni

class CameraOptions internal constructor(
    createNative: () -> Long = ::nativeNew
) : NativeObject(
    new = createNative,
    destroy = ::nativeDestroy
) {
    constructor() : this(::nativeNew)
    fun withCenter(center: LatLng?): CameraOptions {
        nativeSetCenter(nativePtr, center)
        return this
    }

    fun withPadding(padding: EdgeInsets?): CameraOptions {
        nativeSetPadding(nativePtr, padding)
        return this
    }

    fun withAnchor(anchor: ScreenCoordinate?): CameraOptions {
        nativeSetAnchor(nativePtr, anchor)
        return this
    }

    fun withZoom(zoom: Double?): CameraOptions {
        if (zoom == null) {
            nativeSetZoomOptional(nativePtr, 0.0, false)
        } else {
            nativeSetZoomOptional(nativePtr, zoom, true)
        }
        return this
    }

    fun withBearing(bearing: Double?): CameraOptions {
        if (bearing == null) {
            nativeSetBearingOptional(nativePtr, 0.0, false)
        } else {
            nativeSetBearingOptional(nativePtr, bearing, true)
        }
        return this
    }

    fun withPitch(pitch: Double?): CameraOptions {
        if (pitch == null) {
            nativeSetPitchOptional(nativePtr, 0.0, false)
        } else {
            nativeSetPitchOptional(nativePtr, pitch, true)
        }
        return this
    }

    val center: LatLng? get() = nativeGetCenter(nativePtr)

    val padding: EdgeInsets? get() = nativeGetPadding(nativePtr)

    val anchor: ScreenCoordinate? get() = nativeGetAnchor(nativePtr)

    val zoom: Double? get() = nativeGetZoomOptional(nativePtr)

    val bearing: Double? get() = nativeGetBearingOptional(nativePtr)

    val pitch: Double? get() = nativeGetPitchOptional(nativePtr)

    override fun equals(other: Any?): Boolean {
        if (this === other) return true
        if (other !is CameraOptions) return false
        return nativeEquals(nativePtr, other.nativePtr)
    }

    override fun hashCode(): Int {
        var result = center?.hashCode() ?: 0
        result = 31 * result + (padding?.hashCode() ?: 0)
        result = 31 * result + (anchor?.hashCode() ?: 0)
        result = 31 * result + (zoom?.hashCode() ?: 0)
        result = 31 * result + (bearing?.hashCode() ?: 0)
        result = 31 * result + (pitch?.hashCode() ?: 0)
        return result
    }

    private companion object {
        init {
            MapLibreNativeLoader.load()
        }

        @JvmStatic external fun nativeNew(): Long
        @JvmStatic external fun nativeDestroy(ptr: Long)
        @JvmStatic external fun nativeSetCenter(ptr: Long, center: LatLng?)
        @JvmStatic external fun nativeSetPadding(ptr: Long, padding: EdgeInsets?)
        @JvmStatic external fun nativeSetAnchor(ptr: Long, anchor: ScreenCoordinate?)
        @JvmStatic external fun nativeSetZoomOptional(ptr: Long, zoom: Double, hasValue: Boolean)
        @JvmStatic external fun nativeSetBearingOptional(ptr: Long, bearing: Double, hasValue: Boolean)
        @JvmStatic external fun nativeSetPitchOptional(ptr: Long, pitch: Double, hasValue: Boolean)
        @JvmStatic external fun nativeGetCenter(ptr: Long): LatLng?
        @JvmStatic external fun nativeGetPadding(ptr: Long): EdgeInsets?
        @JvmStatic external fun nativeGetAnchor(ptr: Long): ScreenCoordinate?
        @JvmStatic external fun nativeGetZoomOptional(ptr: Long): Double?
        @JvmStatic external fun nativeGetBearingOptional(ptr: Long): Double?
        @JvmStatic external fun nativeGetPitchOptional(ptr: Long): Double?
        @JvmStatic external fun nativeEquals(ptr: Long, otherPtr: Long): Boolean
    }
}
