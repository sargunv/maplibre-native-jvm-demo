package com.maplibre.jni

/**
 * Platform-specific utility functions
 */
object PlatformUtils {
    init {
        // Ensure the native library is loaded before calling any native methods
        MapLibreNativeLoader.load()
    }

    /**
     * Get the system's pixel ratio (DPI scaling factor).
     * 
     * On macOS: Returns the backing scale factor from NSScreen
     * On Linux: Returns Xft.dpi / 96.0 from X resources
     * On Windows: Returns 1.0 (not yet implemented)
     * 
     * @return The system pixel ratio, typically 1.0 for standard DPI or 2.0 for HiDPI
     */
    @JvmStatic
    external fun getSystemPixelRatio(): Float
}