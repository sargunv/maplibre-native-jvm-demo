package com.maplibre.jni

import java.nio.file.Files
import java.nio.file.StandardCopyOption
import java.lang.ref.Cleaner

/**
 * Handles loading the native MapLibre JNI library.
 */
internal object MapLibreNativeLoader {
    private var loaded = false
    
    @Synchronized
    fun load() {
        if (loaded) return
        
        val osName = System.getProperty("os.name").lowercase()
        val osArch = System.getProperty("os.arch")
        
        val libName = when {
            osName.contains("linux") -> "libmaplibre-jni.so"
            osName.contains("mac") -> "libmaplibre-jni.dylib"
            osName.contains("win") -> "maplibre-jni.dll"
            else -> throw UnsupportedOperationException("Unsupported OS: $osName")
        }
        
        val osDir = when {
            osName.contains("linux") -> "linux"
            osName.contains("mac") -> "macos"
            osName.contains("win") -> "windows"
            else -> throw UnsupportedOperationException("Unsupported OS: $osName")
        }
        
        // Load from resources
        val resourcePath = "/native/$osDir/$libName"
        val inputStream = MapLibreNativeLoader::class.java.getResourceAsStream(resourcePath)
            ?: throw RuntimeException("Native library not found: $resourcePath")
            
        val tempFile = Files.createTempFile("maplibre-jni", libName.substringAfter("."))
        inputStream.use { input ->
            Files.copy(input, tempFile, StandardCopyOption.REPLACE_EXISTING)
        }
        
        System.load(tempFile.toAbsolutePath().toString())
        loaded = true
    }
}

open class NativeObject internal constructor(
    new: () -> Long,
    destroy: (Long) -> Unit
) : AutoCloseable {

    private val _nativePtr: Long = new()
    internal val nativePtr: Long
        get() = if (!state.isCleaned) _nativePtr else throw IllegalStateException("Native object used after cleanup")
    
    // Use a separate object to hold state that can be safely accessed from cleaner
    private class State(@Volatile var isCleaned: Boolean = false)
    private val state = State()
    
    private val cleanable = cleaner.register(this) {
        if (!state.isCleaned) {
            destroy(_nativePtr)
            state.isCleaned = true
        }
    }
    
    /**
     * Secondary constructor for creating from an existing native pointer.
     * Used when C++ returns a new object (e.g., LatLng::wrapped()).
     */
    protected constructor(ptr: Long, destroy: (Long) -> Unit) : this({ ptr }, destroy)

    override fun close() {
        synchronized(state) {
            if (!state.isCleaned) {
                cleanable.clean()
                state.isCleaned = true
            }
        }
    }

    private companion object {
        val cleaner = Cleaner.create()
    }
}
