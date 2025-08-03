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
        
        // Initialize when canvas is ready
        canvas.addComponentListener(object : ComponentAdapter() {
            var initialized = false
            
            override fun componentResized(e: ComponentEvent) {
                if (!initialized && canvas.width > 0 && canvas.height > 0) {
                    initialized = true
                    initializeMapLibre()
                } else if (initialized) {
                    // Update size with proper scaling for Retina displays
                    val scale = canvas.graphicsConfiguration?.defaultTransform?.scaleX?.toFloat() ?: 1.0f
                    val pixelWidth = (canvas.width * scale).toInt()
                    val pixelHeight = (canvas.height * scale).toInt()
                    backend?.updateSize(pixelWidth, pixelHeight)
                    map?.setSize(Size(pixelWidth, pixelHeight))
                }
            }
            
            fun initializeMapLibre() {
                try {
                    runLoop = RunLoop()
                    
                    // Create JAWT backend (automatically selects Metal on macOS, OpenGL ES on Linux/Windows)
                    // On macOS with Retina displays, we need to use the actual pixel dimensions
                    val scale = canvas.graphicsConfiguration?.defaultTransform?.scaleX?.toFloat() ?: 1.0f
                    val pixelWidth = (canvas.width * scale).toInt()
                    val pixelHeight = (canvas.height * scale).toInt()
                    
                    println("Canvas size: ${canvas.width}x${canvas.height}, scale: $scale, pixel size: ${pixelWidth}x${pixelHeight}")
                    
                    backend = JAWTRendererBackend(canvas, pixelWidth, pixelHeight)
                    frontend = RendererFrontend(backend.getRendererBackend(), scale)
                    
                    val observer = object : MapObserver {
                        override fun onCameraWillChange(mode: MapObserver.CameraChangeMode) {
                            println("Camera will change: $mode")
                        }
                        
                        override fun onCameraIsChanging() {
                            println("Camera is changing")
                        }
                        
                        override fun onCameraDidChange(mode: MapObserver.CameraChangeMode) {
                            println("Camera changed: $mode")
                        }
                        
                        override fun onWillStartLoadingMap() {
                            println("Will start loading map")
                        }
                        
                        override fun onDidFinishLoadingMap() {
                            println("Did finish loading map")
                        }
                        
                        override fun onDidFailLoadingMap(error: MapLoadError, message: String) {
                            println("❌ Map loading failed: $error - $message")
                        }
                        
                        override fun onStyleImageMissing(imageId: String) {
                            println("⚠️ Style image missing: $imageId")
                        }
                        
                        override fun onWillStartRenderingFrame() {
                            // println("Will start rendering frame")
                        }
                        
                        override fun onDidFinishRenderingFrame(status: MapObserver.RenderFrameStatus) {
                            // println("Did finish rendering frame: $status")
                            // Don't call canvas.repaint() - it causes flickering
                        }
                        
                        override fun onWillStartRenderingMap() {
                            println("Will start rendering map")
                        }
                        
                        override fun onDidFinishRenderingMap(mode: MapObserver.RenderMode) {
                            println("Did finish rendering map in mode: $mode")
                        }
                        
                        override fun onDidFinishLoadingStyle() {
                            println("Map style loaded")
                            // Don't call canvas.repaint() - it causes flickering
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
                        .withPixelRatio(1.0f)
                        .withSize(Size(canvas.width, canvas.height))
                    
                    map = MaplibreMap(
                        rendererFrontend = frontend,
                        mapObserver = observer,
                        mapOptions = mapOptions,
                        resourceOptions = resourceOptions,
                        clientOptions = clientOptions
                    )
                    
                    // Activate file sources for network loading
                    map?.activateFileSources()
                    
                    // Load a style
                    val styleUrl = "https://tiles.openfreemap.org/styles/bright"
                    map?.loadStyleURL(styleUrl)
                    
                    // Set camera to show San Francisco
                    val sfCenter = LatLng(37.7749, -122.4194) // San Francisco coordinates
                    val cameraOptions = CameraOptions()
                        .withCenter(sfCenter)
                        .withZoom(11.0) // City-level zoom
                        .withBearing(0.0)
                        .withPitch(0.0)
                    map?.jumpTo(cameraOptions)
                    
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
        
        // Create render timer
        val renderTimer = Timer(16) { // ~60 FPS
            runLoop?.runOnce()
            frontend?.render()
            // Don't call swap() - it's handled internally by the renderer
            // Don't call canvas.repaint() - it might cause AWT to clear the buffer
        }
        
        // Create window
        val frame = JFrame("MapLibre Native JVM Demo")
        frame.defaultCloseOperation = JFrame.EXIT_ON_CLOSE
        frame.layout = BorderLayout()
        frame.add(canvas, BorderLayout.CENTER)
        frame.setSize(800, 600)
        frame.setLocationRelativeTo(null)
        
        frame.addWindowListener(object : WindowAdapter() {
            override fun windowClosing(e: WindowEvent) {
                renderTimer.stop()
                
                // Cleanup
                map?.close()
                frontend?.close()
                backend?.close()
                runLoop?.close()
                
                exitProcess(0)
            }
        })
        
        frame.isVisible = true
        renderTimer.start()
    }
}