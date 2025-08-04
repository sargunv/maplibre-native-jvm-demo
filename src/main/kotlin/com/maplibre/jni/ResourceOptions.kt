package com.maplibre.jni

/**
 * Configuration options for resource loading and caching.
 * This controls how MapLibre fetches and caches map resources.
 */
data class ResourceOptions(
    val apiKey: String = "",
    val tileServerOptions: TileServerOptions = TileServerOptions.default(),
    val cachePath: String = "",
    val assetPath: String = "",
    val maximumCacheSize: Long = 50 * 1024 * 1024 // 50 MB default
) {
    init {
        require(maximumCacheSize >= 0) { "maximumCacheSize must be non-negative" }
    }
    
    /**
     * Creates a copy with a different API key.
     */
    fun withApiKey(apiKey: String) = copy(apiKey = apiKey)
    
    /**
     * Creates a copy with different tile server options.
     */
    fun withTileServerOptions(options: TileServerOptions) = copy(tileServerOptions = options)
    
    /**
     * Creates a copy with a different cache path.
     */
    fun withCachePath(path: String) = copy(cachePath = path)
    
    /**
     * Creates a copy with a different asset path.
     */
    fun withAssetPath(path: String) = copy(assetPath = path)
    
    /**
     * Creates a copy with a different maximum cache size.
     */
    fun withMaximumCacheSize(sizeInBytes: Long) = copy(maximumCacheSize = sizeInBytes)
    
    companion object {
        /**
         * Default resource options with MapLibre demo tiles configuration.
         */
        fun default() = ResourceOptions()
    }
}