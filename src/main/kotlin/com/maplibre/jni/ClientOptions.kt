package com.maplibre.jni

class ClientOptions internal constructor(
    private val createNative: () -> Long = ::nativeNew
) : NativeObject(
    new = createNative,
    destroy = ::nativeDestroy
) {
    constructor() : this(::nativeNew)
    
    fun withName(name: String): ClientOptions {
        nativeSetName(nativePtr, name)
        return this
    }
    
    fun withVersion(version: String): ClientOptions {
        nativeSetVersion(nativePtr, version)
        return this
    }
    
    val name: String get() = nativeGetName(nativePtr)
    
    val version: String get() = nativeGetVersion(nativePtr)
    
    fun clone(): ClientOptions {
        return ClientOptions { nativeClone(nativePtr) }
    }
    
    override fun equals(other: Any?): Boolean {
        if (this === other) return true
        if (other !is ClientOptions) return false
        return name == other.name && version == other.version
    }
    
    override fun hashCode(): Int {
        var result = name.hashCode()
        result = 31 * result + version.hashCode()
        return result
    }
    
    override fun toString(): String {
        return "ClientOptions(name='$name', version='$version')"
    }
    
    private companion object {
        init {
            MapLibreNativeLoader.load()
        }
        
        @JvmStatic external fun nativeNew(): Long
        @JvmStatic external fun nativeDestroy(ptr: Long)
        @JvmStatic external fun nativeClone(ptr: Long): Long
        @JvmStatic external fun nativeSetName(ptr: Long, name: String)
        @JvmStatic external fun nativeSetVersion(ptr: Long, version: String)
        @JvmStatic external fun nativeGetName(ptr: Long): String
        @JvmStatic external fun nativeGetVersion(ptr: Long): String
    }
}