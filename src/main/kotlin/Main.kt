import com.maplibre.jni.*
import java.awt.BorderLayout
import java.awt.Canvas
import java.awt.event.ComponentAdapter
import java.awt.event.ComponentEvent
import java.awt.event.WindowAdapter
import java.awt.event.WindowEvent
import javax.swing.JFrame
import javax.swing.SwingUtilities
import javax.swing.Timer
import kotlin.system.exitProcess

fun main() {
  SwingUtilities.invokeLater {
    val canvas = Canvas()
    var map: MaplibreMap? = null

    canvas.addComponentListener(object : ComponentAdapter() {
      override fun componentResized(e: ComponentEvent) {
        if (canvas.width > 0 && canvas.height > 0) {
          initializeMapLibre()
          canvas.removeComponentListener(this)
        }
      }

      private fun initializeMapLibre() {
        try {
          val pixelRatio =
            canvas.graphicsConfiguration!!.defaultTransform.scaleX.toFloat()
          map = MaplibreMap(
            canvas = canvas,
            mapObserver = DemoObserver(),
            mapOptions = MapOptions(
              mapMode = MapMode.CONTINUOUS,
              viewportMode = ViewportMode.DEFAULT,
              constrainMode = ConstrainMode.HEIGHT_ONLY,
              crossSourceCollisions = true,
              pixelRatio = pixelRatio,
              size = Size(
                width = (canvas.width * pixelRatio).toInt(),
                height = (canvas.height * pixelRatio).toInt()
              )
            ),
            resourceOptions = ResourceOptions(
              apiKey = "",
              assetPath = "build/",
              cachePath = "build/maplibre-cache"
            ),
            clientOptions = ClientOptions(
              name = "MapLibreJNIDemo",
              version = "0.0.0",
            )
          )

          map.loadStyleURL("https://tiles.openfreemap.org/styles/bright")

          val cameraOptions = CameraOptions(
            center = LatLng(37.7749, -122.4194), // San Francisco coordinates
            zoom = 13.0,
            bearing = 0.0,
            pitch = 0.0
          )
          map.flyTo(cameraOptions)
        } catch (e: Exception) {
          println("Failed to initialize MapLibre: ${e.message}")
          e.printStackTrace()
        }
      }
    })

    val frame = JFrame("MapLibre Native JVM Demo")

    frame.isVisible = true
    frame.defaultCloseOperation = JFrame.EXIT_ON_CLOSE
    frame.layout = BorderLayout()
    frame.add(canvas, BorderLayout.CENTER)
    frame.setSize(800, 600)
    frame.setLocationRelativeTo(null)

    frame.addWindowListener(object : WindowAdapter() {
      override fun windowClosing(e: WindowEvent) {
        exitProcess(0)
      }
    })

    Timer(16) { // ~60 FPS
      map?.tick()
    }.start()
  }
}

