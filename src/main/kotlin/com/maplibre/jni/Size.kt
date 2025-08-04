package com.maplibre.jni

data class Size(
    val width: Int,
    val height: Int
) {
    val area: Int get() = width * height
    val aspectRatio: Float get() = if (height != 0) width.toFloat() / height else 0f
    val isEmpty: Boolean get() = width == 0 || height == 0
    
    override fun toString(): String = "Size($width, $height)"
}