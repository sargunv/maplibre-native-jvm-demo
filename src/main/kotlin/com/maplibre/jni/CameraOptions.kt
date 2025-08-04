package com.maplibre.jni

data class CameraOptions(
    val center: LatLng? = null,
    val padding: EdgeInsets? = null,
    val anchor: ScreenCoordinate? = null,
    val zoom: Double? = null,
    val bearing: Double? = null,
    val pitch: Double? = null
) {
    fun withCenter(center: LatLng?) = copy(center = center)
    fun withPadding(padding: EdgeInsets?) = copy(padding = padding)
    fun withAnchor(anchor: ScreenCoordinate?) = copy(anchor = anchor)
    fun withZoom(zoom: Double?) = copy(zoom = zoom)
    fun withBearing(bearing: Double?) = copy(bearing = bearing)
    fun withPitch(pitch: Double?) = copy(pitch = pitch)
}