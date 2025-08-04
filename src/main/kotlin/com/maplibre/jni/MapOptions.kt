package com.maplibre.jni

/**
 * Configuration options for initializing a Map.
 * This is a simple data holder that specifies various map settings.
 */
data class MapOptions(
    val mapMode: MapMode = MapMode.CONTINUOUS,
    val constrainMode: ConstrainMode = ConstrainMode.HEIGHT_ONLY,
    val viewportMode: ViewportMode = ViewportMode.DEFAULT,
    val crossSourceCollisions: Boolean = true,
    val northOrientation: NorthOrientation = NorthOrientation.UPWARDS,
    val size: Size = Size(64, 64),
    val pixelRatio: Float = 1.0f
) {
    init {
        require(pixelRatio > 0) { "pixelRatio must be positive" }
        require(!size.isEmpty) { "size must not be empty" }
    }
    
    /**
     * Creates a copy with a different map mode.
     */
    fun withMapMode(mode: MapMode) = copy(mapMode = mode)
    
    /**
     * Creates a copy with a different constrain mode.
     */
    fun withConstrainMode(mode: ConstrainMode) = copy(constrainMode = mode)
    
    /**
     * Creates a copy with a different viewport mode.
     */
    fun withViewportMode(mode: ViewportMode) = copy(viewportMode = mode)
    
    /**
     * Creates a copy with cross-source collisions enabled/disabled.
     */
    fun withCrossSourceCollisions(enabled: Boolean) = copy(crossSourceCollisions = enabled)
    
    /**
     * Creates a copy with a different north orientation.
     */
    fun withNorthOrientation(orientation: NorthOrientation) = copy(northOrientation = orientation)
    
    /**
     * Creates a copy with a different size.
     */
    fun withSize(size: Size) = copy(size = size)
    
    /**
     * Creates a copy with a different pixel ratio.
     */
    fun withPixelRatio(ratio: Float) = copy(pixelRatio = ratio)
}