package com.maplibre.jni

data class CameraOptions(
    val center: LatLng? = null,
    val padding: EdgeInsets? = null,
    val anchor: ScreenCoordinate? = null,
    val zoom: Double? = null,
    val bearing: Double? = null,
    val pitch: Double? = null
)
