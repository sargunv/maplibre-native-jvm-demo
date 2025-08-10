import org.maplibre.kmp.native.MapDebugOptions

/**
 * Examples of using the MapDebugOptions value class
 */
fun mapDebugOptionsExamples() {
    // Create with constructor
    val options1 = MapDebugOptions(
        tileBorders = true,
        collision = true
    )
    
    // Use predefined constants
    val options2 = MapDebugOptions.TILE_BORDERS
    
    // Combine options with + operator
    val options3 = MapDebugOptions.TILE_BORDERS + MapDebugOptions.COLLISION
    
    // Remove options with - operator
    val options4 = options3 - MapDebugOptions.COLLISION
    
    // Copy with modifications
    val options5 = options1.copy(
        timestamps = true,
        collision = false
    )
    
    // Check individual flags
    if (options1.tileBorders) {
        println("Tile borders are enabled")
    }
    
    // Check if no debug options are set
    if (MapDebugOptions.NO_DEBUG.isNoDebug) {
        println("No debug options enabled")
    }
    
    // Get string representation
    println(options1) // "TileBorders | Collision"
    println(MapDebugOptions.NO_DEBUG) // "NoDebug"
    
    // Pass directly to map (value is the underlying Int)
    // map.setDebug(options1)
    
    // Create from raw int value (e.g., from getDebug())
    val fromInt = MapDebugOptions(0b00010010) // TileBorders | Collision
}
