package com.maplibre.jni

/**
 * Camera position and orientation options.
 * 
 * Note: `center` and `anchor` are mutually exclusive in MapLibre Native.
 * Use the companion object factory methods to create instances correctly.
 */
data class CameraOptions(
    val center: LatLng? = null,
    val padding: EdgeInsets? = null,
    val anchor: ScreenCoordinate? = null,
    val zoom: Double? = null,
    val bearing: Double? = null,
    val pitch: Double? = null
) {
    companion object {
        /**
         * Creates a camera options with a geographic center point.
         * Use this for absolute positioning of the camera.
         */
        @JvmStatic
        fun centered(
            center: LatLng,
            zoom: Double? = null,
            bearing: Double? = null,
            pitch: Double? = null,
            padding: EdgeInsets? = null
        ): CameraOptions = CameraOptions(
            center = center,
            padding = padding,
            anchor = null,  // Explicitly null when using center
            zoom = zoom,
            bearing = bearing,
            pitch = pitch
        )

        /**
         * Creates a camera options with a screen anchor point.
         * Use this for transformations that should keep a specific screen point fixed.
         */
        @JvmStatic
        fun anchored(
            anchor: ScreenCoordinate,
            zoom: Double? = null,
            bearing: Double? = null,
            pitch: Double? = null,
            padding: EdgeInsets? = null
        ): CameraOptions = CameraOptions(
            center = null,  // Explicitly null when using anchor
            padding = padding,
            anchor = anchor,
            zoom = zoom,
            bearing = bearing,
            pitch = pitch
        )
    }
}
