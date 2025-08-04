import com.maplibre.jni.MapLoadError
import com.maplibre.jni.MapObserver

class DemoObserver : MapObserver {
  override fun onCameraWillChange(mode: MapObserver.CameraChangeMode) {
  }

  override fun onCameraIsChanging() {
  }

  override fun onCameraDidChange(mode: MapObserver.CameraChangeMode) {
  }

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

  override fun onStyleImageMissing(imageId: String) {
    println("⚠️ Style image missing: $imageId")
  }

  override fun onWillStartRenderingFrame() {
  }

  override fun onDidFinishRenderingFrame(status: MapObserver.RenderFrameStatus) {
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
