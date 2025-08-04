package com.maplibre.jni

/**
 * Configuration options for client identification.
 * This is a simple data holder that identifies the client application.
 */
data class ClientOptions(
    val name: String = "MapLibre JVM",
    val version: String = "1.0.0"
) {
    /**
     * Creates a copy with a different name.
     */
    fun withName(name: String) = copy(name = name)
    
    /**
     * Creates a copy with a different version.
     */
    fun withVersion(version: String) = copy(version = version)
}