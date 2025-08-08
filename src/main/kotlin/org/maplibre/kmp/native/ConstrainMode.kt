package org.maplibre.kmp.native

enum class ConstrainMode(val nativeValue: Int) {
    NONE(0),
    HEIGHT_ONLY(1),
    WIDTH_AND_HEIGHT(2),
    SCREEN(3);
    
    companion object {
        fun fromNative(value: Int): ConstrainMode {
            return values().find { it.nativeValue == value }
                ?: throw IllegalArgumentException("Unknown ConstrainMode value: $value")
        }
    }
}
