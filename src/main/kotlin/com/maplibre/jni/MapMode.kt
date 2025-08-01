package com.maplibre.jni

enum class MapMode(val nativeValue: Int) {
    CONTINUOUS(0),
    STATIC(1),
    TILE(2);
    
    companion object {
        fun fromNative(value: Int): MapMode {
            return values().find { it.nativeValue == value }
                ?: throw IllegalArgumentException("Unknown MapMode value: $value")
        }
    }
}