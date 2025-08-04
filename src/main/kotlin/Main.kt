import com.maplibre.jni.*
import java.awt.BorderLayout
import javax.swing.JFrame
import javax.swing.SwingUtilities

fun main() {
  SwingUtilities.invokeLater {
    val canvas = MaplibreCanvas(
      mapObserver = DemoObserver(),
      mapOptions = MapOptions(
        mapMode = MapMode.CONTINUOUS,
        viewportMode = ViewportMode.DEFAULT,
        constrainMode = ConstrainMode.HEIGHT_ONLY,
        crossSourceCollisions = true,
        pixelRatio = 1.0f,
        size = Size(800, 600)
      ),
      resourceOptions = ResourceOptions(
        apiKey = "",
        assetPath = "build/",
        cachePath = "build/maplibre-cache"
      ),
      clientOptions = ClientOptions(
        name = "MapLibreJNIDemo",
        version = "0.0.0",
      ),
      frameRate = 60,
      onMapReady = { map, canvas ->
        map.loadStyleURL("https://tiles.openfreemap.org/styles/bright")
        map.flyTo(
          CameraOptions(
            center = LatLng(37.7749, -122.4194), // San Francisco coordinates
            zoom = 13.0,
            bearing = 0.0,
            pitch = 0.0
          )
        )

        // Enable user interactions
        MapInteractionHandler(canvas, map).enable()
      }
    )

    JFrame("MapLibre Native JVM Demo").apply {
      isVisible = true
      defaultCloseOperation = JFrame.EXIT_ON_CLOSE
      layout = BorderLayout()
      add(canvas, BorderLayout.CENTER)
      setSize(800, 600)
      setLocationRelativeTo(null)
    }
  }
}

