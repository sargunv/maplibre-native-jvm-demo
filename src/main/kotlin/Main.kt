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
import java.awt.Color

fun main() {
  SwingUtilities.invokeLater {
    // Create a simple Canvas (no OpenGL initialization needed!)
    val canvas = Canvas()

    // MapLibre components
    var map: MaplibreMap? = null
    var renderer: AwtCanvasRenderer? = null

    // Spinner for visual feedback
    val spinnerFrames = listOf("⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏")
    var spinnerIndex = 0
    var frameCount = 0L

    // Initialize when canvas is ready
    canvas.addComponentListener(object : ComponentAdapter() {
      var initialized = false

      override fun componentResized(e: ComponentEvent) {
        if (!initialized && canvas.width > 0 && canvas.height > 0) {
          initialized = true
          initializeMapLibre()
        }
        // Update size with proper scaling for Retina displays
        val scale =
          canvas.graphicsConfiguration?.defaultTransform?.scaleX?.toFloat()
            ?: 1.0f
        val pixelWidth = (canvas.width * scale).toInt()
        val pixelHeight = (canvas.height * scale).toInt()
        renderer?.updateSize(pixelWidth, pixelHeight)
        map?.setSize(Size(pixelWidth, pixelHeight))
      }

      fun initializeMapLibre() {
        try {
          // Calculate pixel ratio and dimensions BEFORE creating components
          // This matches how GLFW handles it: framebuffer size / window size
          val scale =
            canvas.graphicsConfiguration?.defaultTransform?.scaleX?.toFloat()
              ?: 1.0f
          val pixelWidth = (canvas.width * scale).toInt()
          val pixelHeight = (canvas.height * scale).toInt()

          println("Canvas size: ${canvas.width}x${canvas.height}, scale: $scale, pixel size: ${pixelWidth}x${pixelHeight}")

          // Create unified renderer with pixel dimensions
          renderer = AwtCanvasRenderer(canvas, pixelWidth, pixelHeight, scale)

          val observer = object : MapObserver {
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

          val resourceOptions = ResourceOptions()
            .withApiKey("")
            .withAssetPath(".")
            .withCachePath("maplibre-cache")

          val clientOptions = ClientOptions()
            .withName("MapLibreJNIDemo")
            .withVersion("1.0.0")

          val mapOptions = MapOptions()
            .withMapMode(MapMode.CONTINUOUS)
            .withViewportMode(ViewportMode.DEFAULT)
            .withConstrainMode(ConstrainMode.HEIGHT_ONLY)
            .withCrossSourceCollisions(true)
            .withPixelRatio(scale)  // Use calculated pixel ratio
            .withSize(Size(pixelWidth, pixelHeight))  // Use pixel dimensions

          map = MaplibreMap(
            renderer = renderer,
            mapObserver = observer,
            mapOptions = mapOptions,
            resourceOptions = resourceOptions,
            clientOptions = clientOptions
          )

          // Activate file sources for network loading
          map.activateFileSources()

          // CRITICAL: Set sizes again after map creation (like GLFW does)
          println("Setting initial size: ${pixelWidth}x${pixelHeight}")
          renderer.updateSize(pixelWidth, pixelHeight)
          map.setSize(Size(pixelWidth, pixelHeight))

          // Load a style
          val styleUrl = "https://tiles.openfreemap.org/styles/bright"
          map.loadStyleURL(styleUrl)

          // Set camera to show San Francisco
          val sfCenter = LatLng(37.7749, -122.4194) // San Francisco coordinates
          val cameraOptions = CameraOptions()
            .withCenter(sfCenter)
            .withZoom(13.0) // City-level zoom
            .withBearing(0.0)
            .withPitch(0.0)
          map.jumpTo(cameraOptions)

          val osName = System.getProperty("os.name")
          val backendName = when {
            osName.contains("Mac", ignoreCase = true) -> "Metal"
            else -> "OpenGL ES"
          }
          println("✅ MapLibre initialized with $backendName backend")

        } catch (e: Exception) {
          println("❌ Failed to initialize MapLibre: ${e.message}")
          e.printStackTrace()
        }
      }
    })

    // Create window
    val frame = JFrame("MapLibre Native JVM Demo")

    // Create render timer - simplified with tick-based approach
    val renderTimer = Timer(16) { // ~60 FPS
      val didRender = renderer?.tick() ?: false

      // Update spinner in title to show rendering activity
      if (didRender) {
        frameCount++
        spinnerIndex = (spinnerIndex + 1) % spinnerFrames.size
        frame.title = "MapLibre Native JVM Demo ${spinnerFrames[spinnerIndex]}"
      }
    }
    frame.isVisible = true
    frame.defaultCloseOperation = JFrame.EXIT_ON_CLOSE
    frame.layout = BorderLayout()
    frame.add(canvas, BorderLayout.CENTER)
    frame.setSize(800, 600)
    frame.setLocationRelativeTo(null)

    frame.addWindowListener(object : WindowAdapter() {
      override fun windowClosing(e: WindowEvent) {
        renderTimer.stop()
        // TODO clean up (currently handled by Cleaner)
        exitProcess(0)
      }
    })

    renderTimer.start()
  }
}
