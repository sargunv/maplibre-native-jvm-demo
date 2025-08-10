package org.maplibre.kmp.native

/**
 * Configuration options for client identification.
 * This is a simple data holder that identifies the client application.
 */
data class ClientOptions(
    val name: String,
    val version: String,
)
