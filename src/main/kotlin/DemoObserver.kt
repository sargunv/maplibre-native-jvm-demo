import org.maplibre.kmp.native.MapLoadError
import org.maplibre.kmp.native.MapObserver

class DemoObserver : MapObserver {
  override fun onWillStartLoadingMap() {
    println("Will start loading map")
  }

  override fun onDidFinishLoadingMap() {
    println("Did finish loading map")
  }

  override fun onDidFailLoadingMap(
      error: MapLoadError,
      message: String
  ) {
    println("❌ Map loading failed: $error - $message")
  }

  override fun onWillStartRenderingMap() {
    println("Will start rendering map")
  }

  override fun onDidFinishRenderingMap(mode: MapObserver.RenderMode) {
    println("Did finish rendering map in mode: $mode")
  }

  override fun onDidFinishLoadingStyle() {
    println("Map style loaded")
  }
}
