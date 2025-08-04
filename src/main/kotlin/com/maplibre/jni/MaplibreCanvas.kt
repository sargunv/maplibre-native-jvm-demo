package com.maplibre.jni

import java.awt.Canvas
import java.awt.event.ComponentAdapter
import java.awt.event.ComponentEvent
import javax.swing.Timer

/**
 * A Canvas that automatically initializes and manages a MapLibre map.
 * This class encapsulates all the initialization logic and provides a callback
 * when the map is ready for configuration.
 */
class MaplibreCanvas(
  private val mapObserver: MapObserver,
  private val mapOptions: MapOptions,
  private val resourceOptions: ResourceOptions,
  private val clientOptions: ClientOptions,
  private val frameRate: Int = 60,
  private val onMapReady: ((MaplibreMap, MaplibreCanvas) -> Unit) = { _, _ -> }
) : Canvas() {

  private var map: MaplibreMap? = null
  private var renderTimer: Timer? = null

  init {
    addComponentListener(object : ComponentAdapter() {
      override fun componentResized(e: ComponentEvent) {
        if (width > 0 && height > 0) {
          initializeMap()
          removeComponentListener(this)
        }
      }
    })
  }

  private fun initializeMap() {
    try {
      val pixelRatio =
        graphicsConfiguration?.defaultTransform?.scaleX?.toFloat() ?: 1.0f

      // Should match the canvas size
      val adjustedMapOptions = mapOptions.copy(
        pixelRatio = pixelRatio,
        size = Size(
          width = (width * pixelRatio).toInt(),
          height = (height * pixelRatio).toInt()
        )
      )

      val map = MaplibreMap(
        canvas = this,
        mapObserver = mapObserver,
        mapOptions = adjustedMapOptions,
        resourceOptions = resourceOptions,
        clientOptions = clientOptions
      ).also { this.map = it }

      startRenderLoop()

      onMapReady(map, this)
    } catch (e: Exception) {
      println("Failed to initialize MapLibre: ${e.message}")
      e.printStackTrace()
    }
  }

  private fun startRenderLoop() {
    renderTimer = Timer(1000 / frameRate) {
      map?.tick()
    }.apply {
      start()
    }
  }

  fun dispose() {
    renderTimer?.stop()
    renderTimer = null
    map = null
  }

  override fun removeNotify() {
    super.removeNotify()
    dispose()
  }
}
