enum class DemoStyle(
    val styleName: String,
    val url: String
) {
    // Protomaps styles
    PROTOMAPS_LIGHT(
        "Protomaps Light",
        "https://api.protomaps.com/styles/v5/light/en.json?key=73c45a97eddd43fb"
    ),
    PROTOMAPS_DARK(
        "Protomaps Dark",
        "https://api.protomaps.com/styles/v5/dark/en.json?key=73c45a97eddd43fb"
    ),
    PROTOMAPS_WHITE(
        "Protomaps White",
        "https://api.protomaps.com/styles/v5/white/en.json?key=73c45a97eddd43fb"
    ),
    PROTOMAPS_GRAYSCALE(
        "Protomaps Grayscale",
        "https://api.protomaps.com/styles/v5/grayscale/en.json?key=73c45a97eddd43fb"
    ),
    PROTOMAPS_BLACK(
        "Protomaps Black",
        "https://api.protomaps.com/styles/v5/black/en.json?key=73c45a97eddd43fb"
    ),
    
    // OpenFreeMap styles
    OPENFREEMAP_BRIGHT(
        "OpenFreeMap Bright",
        "https://tiles.openfreemap.org/styles/bright"
    ),
    OPENFREEMAP_LIBERTY(
        "OpenFreeMap Liberty",
        "https://tiles.openfreemap.org/styles/liberty"
    ),
    OPENFREEMAP_POSITRON(
        "OpenFreeMap Positron",
        "https://tiles.openfreemap.org/styles/positron"
    ),
    
    // MapLibre Demo Tiles
    MAPLIBRE_DEMO_TILES(
        "MapLibre Demo Tiles",
        "https://demotiles.maplibre.org/style.json"
    );
    
    companion object {
        val DEFAULT = OPENFREEMAP_BRIGHT
        
        fun getByIndex(index: Int): DemoStyle {
            val values = entries.toTypedArray()
            return values[index % values.size]
        }
    }
}