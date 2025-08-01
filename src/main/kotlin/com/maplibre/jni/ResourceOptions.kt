package com.maplibre.jni

class ResourceOptions internal constructor(
    private val createNative: () -> Long = { nativeDefault() }
) : NativeObject(
    new = createNative,
    destroy = ::nativeDestroy
) {
    constructor() : this({ nativeDefault() })
    
    fun withApiKey(apiKey: String): ResourceOptions {
        nativeSetApiKey(nativePtr, apiKey)
        return this
    }
    
    // TODO: Implement withTileServerOptions - requires TileServerOptions wrapper
    // For now, users can rely on the default tile server configuration
    
    fun withCachePath(path: String): ResourceOptions {
        nativeSetCachePath(nativePtr, path)
        return this
    }
    
    fun withAssetPath(path: String): ResourceOptions {
        nativeSetAssetPath(nativePtr, path)
        return this
    }
    
    fun withMaximumCacheSize(sizeInBytes: Long): ResourceOptions {
        require(sizeInBytes >= 0) { "maximumCacheSize must be non-negative" }
        nativeSetMaximumCacheSize(nativePtr, sizeInBytes)
        return this
    }
    
    // TODO: Implement withPlatformContext - requires platform-specific handling
    // This is rarely needed and platform-dependent
    
    val apiKey: String get() = nativeGetApiKey(nativePtr)
    
    // TODO: Implement tileServerOptions getter - requires TileServerOptions wrapper
    
    val cachePath: String get() = nativeGetCachePath(nativePtr)
    
    val assetPath: String get() = nativeGetAssetPath(nativePtr)
    
    val maximumCacheSize: Long get() = nativeGetMaximumCacheSize(nativePtr)
    
    // TODO: Implement platformContext getter - platform-specific
    
    fun clone(): ResourceOptions {
        return ResourceOptions { nativeClone(nativePtr) }
    }
    
    override fun equals(other: Any?): Boolean {
        if (this === other) return true
        if (other !is ResourceOptions) return false
        // Compare the fields we can access
        return apiKey == other.apiKey &&
               cachePath == other.cachePath &&
               assetPath == other.assetPath &&
               maximumCacheSize == other.maximumCacheSize
    }
    
    override fun hashCode(): Int {
        var result = apiKey.hashCode()
        result = 31 * result + cachePath.hashCode()
        result = 31 * result + assetPath.hashCode()
        result = 31 * result + maximumCacheSize.hashCode()
        return result
    }
    
    override fun toString(): String {
        return "ResourceOptions(apiKey='$apiKey', cachePath='$cachePath', assetPath='$assetPath', maximumCacheSize=$maximumCacheSize)"
    }
    
    private companion object {
        init {
            MapLibreNativeLoader.load()
        }
        
        @JvmStatic external fun nativeDefault(): Long
        @JvmStatic external fun nativeDestroy(ptr: Long)
        @JvmStatic external fun nativeClone(ptr: Long): Long
        @JvmStatic external fun nativeSetApiKey(ptr: Long, apiKey: String)
        @JvmStatic external fun nativeSetCachePath(ptr: Long, path: String)
        @JvmStatic external fun nativeSetAssetPath(ptr: Long, path: String)
        @JvmStatic external fun nativeSetMaximumCacheSize(ptr: Long, size: Long)
        @JvmStatic external fun nativeGetApiKey(ptr: Long): String
        @JvmStatic external fun nativeGetCachePath(ptr: Long): String
        @JvmStatic external fun nativeGetAssetPath(ptr: Long): String
        @JvmStatic external fun nativeGetMaximumCacheSize(ptr: Long): Long
    }
}