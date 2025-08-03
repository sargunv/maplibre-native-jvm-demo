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
    // Create a simple Canvas (no OpenGL initialization needed!)
    val canvas = Canvas()

    // MapLibre components
    var runLoop: RunLoop? = null
    var map: MaplibreMap? = null
    var backend: JAWTRendererBackend? = null
    var frontend: RendererFrontend? = null
    
    // Rendering state
    var dirty = false
    val invalidate = { dirty = true }
    
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
        } else if (initialized) {
          // Update size with proper scaling for Retina displays
          val scale =
            canvas.graphicsConfiguration?.defaultTransform?.scaleX?.toFloat()
              ?: 1.0f
          val pixelWidth = (canvas.width * scale).toInt()
          val pixelHeight = (canvas.height * scale).toInt()
          backend?.updateSize(pixelWidth, pixelHeight)
          map?.setSize(Size(pixelWidth, pixelHeight))
          invalidate() // Request render after resize
        }
      }

      fun initializeMapLibre() {
        try {
          runLoop = RunLoop()

          // Calculate pixel ratio and dimensions BEFORE creating components
          // This matches how GLFW handles it: framebuffer size / window size
          val scale =
            canvas.graphicsConfiguration?.defaultTransform?.scaleX?.toFloat()
              ?: 1.0f
          val pixelWidth = (canvas.width * scale).toInt()
          val pixelHeight = (canvas.height * scale).toInt()

          println("Canvas size: ${canvas.width}x${canvas.height}, scale: $scale, pixel size: ${pixelWidth}x${pixelHeight}")

          // Create backend with pixel dimensions (framebuffer size)
          backend = JAWTRendererBackend(canvas, pixelWidth, pixelHeight)

          // Create frontend with proper pixel ratio and update callback
          frontend = RendererFrontend(backend.getRendererBackend(), scale, invalidate)

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
            rendererFrontend = frontend,
            mapObserver = observer,
            mapOptions = mapOptions,
            resourceOptions = resourceOptions,
            clientOptions = clientOptions
          )

          // Activate file sources for network loading
          map.activateFileSources()

          // CRITICAL: Set sizes again after map creation (like GLFW does)
          // GLFW sets both backend and map sizes after creation
          println("Setting initial size: ${pixelWidth}x${pixelHeight}")
          backend.updateSize(pixelWidth, pixelHeight)
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
    
    // Create render timer
    val renderTimer = Timer(16) { // ~60 FPS
      runLoop?.runOnce()
      
      // Only render when dirty (map has changes)
      if (dirty && frontend != null) {
        dirty = false
        frontend.render()
        
        // Update spinner in title to show rendering activity
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
