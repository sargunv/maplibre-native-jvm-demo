import com.maplibre.jni.*
import java.awt.BorderLayout
import java.awt.event.WindowAdapter
import java.awt.event.WindowEvent
import javax.swing.JFrame
import javax.swing.SwingUtilities
import kotlin.system.exitProcess

fun main() {
  // Disable AWT background erase to prevent resize flickering
  // See: https://forum.jogamp.org/Resize-flickering-with-AWT-GLCanvas-and-JOGL-2-4-0-td4042181.html
  System.setProperty("sun.awt.noerasebackground", "true")
  
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
      frameRate = 120,
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

    val frame = JFrame("MapLibre Native JVM Demo").apply {
      defaultCloseOperation = JFrame.EXIT_ON_CLOSE
      layout = BorderLayout()
      add(canvas, BorderLayout.CENTER)
      setSize(800, 600)
      setLocationRelativeTo(null)
      
      addWindowListener(object : WindowAdapter() {
        override fun windowClosing(e: WindowEvent) {
          this@apply.dispose()
          System.exit(0)
        }
      })
      
      isVisible = true
    }
  }
}

