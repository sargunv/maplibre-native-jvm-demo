package org.maplibre.kmp.native.internal

import java.awt.Canvas
import org.maplibre.kmp.native.MapLibreNativeLoader

object Native {
  init {
    MapLibreNativeLoader.load()
  }

  @JvmStatic external fun createJniRendererFrontend(kotlinFrontend: Any): Long
  @JvmStatic external fun destroyJniRendererFrontend(ptr: Long)

  @JvmStatic external fun extractMetalLayerSurface(canvas: Canvas, width: Int, height: Int): Long
  @JvmStatic external fun destroySurfaceDescriptor(ptr: Long)

  @JvmStatic external fun createMetalBackend(surfaceDescPtr: Long, width: Int, height: Int, pixelRatio: Float, contextMode: Int): Long
  @JvmStatic external fun backendSetSize(backendPtr: Long, width: Int, height: Int)
  @JvmStatic external fun backendGetThreadPoolPtr(backendPtr: Long): Long

  @JvmStatic external fun rendererCreate(backendPtr: Long, pixelRatio: Float, localFontFamily: String?): Long
  @JvmStatic external fun rendererDestroy(rendererPtr: Long)
  @JvmStatic external fun rendererRenderWithFrontend(rendererPtr: Long, frontendPtr: Long, paramsId: Long)
  @JvmStatic external fun rendererRenderWithBackend(rendererPtr: Long, frontendPtr: Long, backendPtr: Long, paramsId: Long)
  @JvmStatic external fun rendererBindObserver(rendererPtr: Long, frontendPtr: Long)

  @JvmStatic external fun nativeNewWithFrontend(frontendPtr: Long, mapObserver: Any, mapOptions: Any, resourceOptions: Any, clientOptions: Any): Long
  @JvmStatic external fun nativeDestroyWithFrontend(ptr: Long)

  // Pump mbgl RunLoop once on the frontend's dedicated loop
  @JvmStatic external fun runLoopOnce(frontendPtr: Long)
}
