package org.maplibre.kmp.native

/**
 * Configuration options for resource loading and caching.
 * This controls how MapLibre fetches and caches map resources.
 */
data class ResourceOptions(
    val apiKey: String = "",
    val tileServerOptions: TileServerOptions = TileServerOptions.DemoTiles,
    val cachePath: String = "maplibre-cache",
    val assetPath: String = "",
    val maximumCacheSize: Long = 50 * 1024 * 1024 // 50 MB default
) {
    init {
        require(maximumCacheSize >= 0) { "maximumCacheSize must be non-negative" }
    }
}
