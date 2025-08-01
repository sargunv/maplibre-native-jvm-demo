package com.maplibre.jni

/**
 * Interface for OpenGL context operations required by MapLibre.
 * Implementations should wrap platform-specific GL context management.
 */
interface GLContext {
    /**
     * Makes this GL context current on the calling thread.
     * @throws RuntimeException if the operation fails
     */
    fun makeCurrent()
    
    /**
     * Releases this GL context from the current thread.
     */
    fun release()
    
    /**
     * Gets the address of an OpenGL extension function.
     * @param name The name of the function
     * @return The function address, or 0 if not found
     */
    fun getProcAddress(name: String): Long
}