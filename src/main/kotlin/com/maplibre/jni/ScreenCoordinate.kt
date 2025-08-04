package com.maplibre.jni

data class ScreenCoordinate(
    val x: Double,
    val y: Double
) {
    override fun toString(): String = "ScreenCoordinate($x, $y)"
}