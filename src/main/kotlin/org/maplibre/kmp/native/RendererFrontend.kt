package org.maplibre.kmp.native

import org.maplibre.kmp.native.internal.Native

interface RendererFrontend {
  fun reset()
  fun setObserver(observer: MapObserver)
  fun onUpdateFromNative(paramsId: Long)
  fun getThreadPoolNativePtr(): Long
}
