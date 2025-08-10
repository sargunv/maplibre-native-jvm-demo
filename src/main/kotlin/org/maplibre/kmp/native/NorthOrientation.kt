package org.maplibre.kmp.native

enum class NorthOrientation(val nativeValue: Int) {
    UPWARDS(0),
    RIGHTWARDS(1),
    DOWNWARDS(2),
    LEFTWARDS(3);
    
    companion object {
        fun fromNative(value: Int): NorthOrientation {
            return values().find { it.nativeValue == value }
                ?: throw IllegalArgumentException("Unknown NorthOrientation value: $value")
        }
    }
}
