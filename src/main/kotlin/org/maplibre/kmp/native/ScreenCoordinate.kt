package org.maplibre.kmp.native

data class ScreenCoordinate(
    val x: Double,
    val y: Double
) {
    override fun toString(): String = "ScreenCoordinate($x, $y)"
}
