package org.maplibre.kmp.native

import org.maplibre.kmp.native.internal.Native
import java.awt.Canvas
import java.awt.event.ComponentAdapter
import java.awt.event.ComponentEvent
import java.util.concurrent.atomic.AtomicBoolean

class CanvasRendererFrontend(
  private val canvas: Canvas,
  private val pixelRatio: Float,
  private val localFontFamily: String? = null,
) : RendererFrontend {
  private var surfaceDescPtr: Long = 0
  private var backendPtr: Long = 0
  internal var rendererPtr: Long = 0
  private var latestParamsId: Long = 0
  private val dirty = AtomicBoolean(false)
  private var mapObserver: MapObserver? = null

  init {
    val w = (canvas.width * pixelRatio).toInt()
    val h = (canvas.height * pixelRatio).toInt()
    surfaceDescPtr = Native.extractMetalLayerSurface(canvas, w, h)
    backendPtr = Native.createMetalBackend(surfaceDescPtr, w, h, pixelRatio, 0)
    rendererPtr = Native.rendererCreate(backendPtr, pixelRatio, localFontFamily)
    // Bind renderer observer to native frontend to receive render callbacks
    // jniFrontendPtr will be populated by MaplibreMap after constructing the frontend; bind when available

    canvas.addComponentListener(object : ComponentAdapter() {
      override fun componentResized(e: ComponentEvent) {
        val scale = canvas.graphicsConfiguration!!.defaultTransform.scaleX.toFloat()
        val pw = (canvas.width * scale).toInt()
        val ph = (canvas.height * scale).toInt()
        Native.backendSetSize(backendPtr, pw, ph)
        dirty.set(true)
      }
    })
  }

  fun tick(): Boolean {
    // Pump the mbgl RunLoop to process network/timer callbacks
    if (jniFrontendPtr != 0L) Native.runLoopOnce(jniFrontendPtr)
    if (!dirty.getAndSet(false)) return false
    val id = latestParamsId
    if (id != 0L) Native.rendererRenderWithBackend(rendererPtr, jniFrontendPtr, backendPtr, id)
    return true
  }

  override fun reset() {
    if (rendererPtr != 0L) {
      Native.rendererDestroy(rendererPtr)
      rendererPtr = 0
    }
    latestParamsId = 0
  }

  override fun setObserver(observer: MapObserver) {
    mapObserver = observer
  }

  override fun onUpdateFromNative(paramsId: Long) {
    latestParamsId = paramsId
    dirty.set(true)
  }

  override fun getThreadPoolNativePtr(): Long {
    return Native.backendGetThreadPoolPtr(backendPtr)
  }

  // JNI renderer frontend pointer we create when wiring Map
  internal var jniFrontendPtr: Long = 0
}
