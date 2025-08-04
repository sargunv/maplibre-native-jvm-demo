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

        // Create a temp directory for all libraries
        val tempDir = Files.createTempDirectory("maplibre-native")

        // On macOS/Windows, we need to extract ANGLE libraries first
        if (osName.contains("mac") || osName.contains("win")) {
            val angleLibs = if (osName.contains("mac")) {
                listOf("libEGL.dylib", "libGLESv2.dylib")
            } else {
                listOf("libEGL.dll", "libGLESv2.dll")
            }

            // Extract ANGLE libraries with their proper names
            for (angleLib in angleLibs) {
                val angleResourcePath = "/native/$osDir/$angleLib"
                val angleInputStream = MapLibreNativeLoader::class.java.getResourceAsStream(angleResourcePath)
                if (angleInputStream != null) {
                    val angleTempFile = tempDir.resolve(angleLib)
                    angleInputStream.use { input ->
                        Files.copy(input, angleTempFile, StandardCopyOption.REPLACE_EXISTING)
                    }
                    // Load the libraries
                    System.load(angleTempFile.toAbsolutePath().toString())
                }
            }
        }

        // Extract and load the main library
        val resourcePath = "/native/$osDir/$libName"
        val inputStream = MapLibreNativeLoader::class.java.getResourceAsStream(resourcePath)
            ?: throw RuntimeException("Native library not found: $resourcePath")

        val tempFile = tempDir.resolve(libName)
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
