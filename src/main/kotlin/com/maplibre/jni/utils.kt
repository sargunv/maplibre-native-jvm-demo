package com.maplibre.jni

import java.nio.file.Files
import java.nio.file.StandardCopyOption
import java.lang.ref.Cleaner

/**
 * Handles loading the native MapLibre JNI library.
 */
internal object MapLibreNativeLoader {
    private var loaded = false

    private val libDeps = mapOf(
        "libGLESv2.dll" to listOf(
            "dxgi",
            "d3d9",
            "VCRUNTIME140",
            "MSVCP140",
        )
    )

    @Synchronized
    fun load() {
        if (loaded) return

        val osName = System.getProperty("os.name").lowercase()
        
        val (osDir, libraries) = when {
            osName.contains("linux") -> "linux" to listOf("libmaplibre-jni.so")
            osName.contains("mac") -> "macos" to listOf("libmaplibre-jni.dylib")
            osName.contains("win") -> "windows" to listOf(
                // Dependencies in load order
                "zlib1.dll",
                "jpeg62.dll",
                "libpng16.dll",
                "libwebpdecoder.dll",
                "icudt74.dll",
                "icuuc74.dll",
                "icuin74.dll",
                "uv.dll",
                "libcurl.dll",
                "libGLESv2.dll",
                "libEGL.dll",
                "maplibre-jni.dll"
            )
            else -> throw UnsupportedOperationException("Unsupported OS: $osName")
        }

        // Create a temp directory for all libraries
        val tempDir = Files.createTempDirectory("maplibre-native")

        // Extract and load all libraries
        for (lib in libraries) {
            // Handle dependencies first
            for (dep in libDeps[lib] ?: emptyList()) {
                System.loadLibrary(dep)
                println("Loaded: $dep")
            }

            val resourcePath = "/native/$osDir/$lib"
            val inputStream = MapLibreNativeLoader::class.java.getResourceAsStream(resourcePath)
            
            if (inputStream != null) {
                val tempFile = tempDir.resolve(lib)
                inputStream.use { input ->
                    Files.copy(input, tempFile, StandardCopyOption.REPLACE_EXISTING)
                }
                
                System.load(tempFile.toAbsolutePath().toString())
                println("Loaded: $resourcePath")
            } else if (lib == libraries.last()) {
                // Main library must exist
                throw RuntimeException("Native library not found: $resourcePath")
            }
        }

        loaded = true
    }
}

open class NativeObject internal constructor(
    new: () -> Long,
    destroy: (Long) -> Unit
) {
    internal val nativePtr: Long = new()

    init {
      cleaner.register(this) {
        destroy(nativePtr)
      }
    }

    private companion object {
        val cleaner: Cleaner = Cleaner.create()

      init {
          MapLibreNativeLoader.load()
      }
    }
}
