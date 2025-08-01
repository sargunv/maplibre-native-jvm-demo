package com.maplibre.jni

enum class ViewportMode(val nativeValue: Int) {
    DEFAULT(0),
    FLIPPED_Y(1);
    
    companion object {
        fun fromNative(value: Int): ViewportMode {
            return values().find { it.nativeValue == value }
                ?: throw IllegalArgumentException("Unknown ViewportMode value: $value")
        }
    }
}