import com.jogamp.opengl.*
import com.jogamp.opengl.awt.GLJPanel
import com.maplibre.jni.*
import java.awt.BorderLayout
import java.awt.event.WindowAdapter
import java.awt.event.WindowEvent
import javax.swing.JFrame
import javax.swing.SwingUtilities
import javax.swing.Timer
import kotlin.system.exitProcess

fun main() {
    SwingUtilities.invokeLater {
        // Use OpenGL ES profile for MapLibre compatibility
        val glProfile = GLProfile.get(GLProfile.GLES3)
        val glCapabilities = GLCapabilities(glProfile)
        
        // Configure framebuffer format
        glCapabilities.setRedBits(8)
        glCapabilities.setGreenBits(8)
        glCapabilities.setBlueBits(8)
        glCapabilities.setAlphaBits(8)
        glCapabilities.setStencilBits(8)
        glCapabilities.setDepthBits(16)
        
        val glPanel = GLJPanel(glCapabilities)
        
        // MapLibre components
        var runLoop: RunLoop? = null
        var map: MaplibreMap? = null
        var backend: JOGLRendererBackend? = null
        var frontend: JOGLRendererFrontend? = null
        var glContext: JOGLContext? = null
        
        glPanel.addGLEventListener(object : GLEventListener {
            override fun init(drawable: GLAutoDrawable) {
                val gl = drawable.gl
                println("OpenGL: ${gl.glGetString(GL.GL_VERSION)} (${gl.glGetString(GL.GL_RENDERER)})")
                
                try {
                    runLoop = RunLoop()
                    
                    glContext = JOGLContext(drawable.context, drawable)
                    backend = JOGLRendererBackend(glContext, drawable.surfaceWidth, drawable.surfaceHeight)
                    frontend = JOGLRendererFrontend(backend, 1.0f)
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
                            if (status.needsRepaint) {
                                glPanel.repaint()
                            }
                        }
                        
                        override fun onWillStartRenderingMap() {
                            println("Will start rendering map")
                        }
                        
                        override fun onDidFinishRenderingMap(mode: MapObserver.RenderMode) {
                            println("Did finish rendering map in mode: $mode")
                        }
                        
                        override fun onDidFinishLoadingStyle() {
                            println("Map style loaded")
                            glPanel.repaint()
                        }
                        
                    }
                    
                    val mapOptions = MapOptions()
                        .withSize(Size(drawable.surfaceWidth, drawable.surfaceHeight))
                        .withPixelRatio(1.0f)
                    
                    val resourceOptions = ResourceOptions()
                        .withCachePath("/tmp/maplibre-cache.db")
                    
                    val clientOptions = ClientOptions()
                        .withName("MapLibre JVM Demo")
                        .withVersion("1.0.0")
                    
                    map = MaplibreMap(frontend, observer, mapOptions, resourceOptions, clientOptions)
                    
                    map.activateFileSources()
                    
                    val styleUrl = "https://tiles.openfreemap.org/styles/bright"
                    map.loadStyleURL(styleUrl)
                    
                    map.jumpTo(CameraOptions()
                        .withCenter(LatLng(37.7749, -122.4194))  // San Francisco
                        .withZoom(12.0))
                    
                    map.triggerRepaint()
                } catch (e: Exception) {
                    e.printStackTrace()
                    println("Failed to initialize map: ${e.message}")
                }
            }
            
            override fun display(drawable: GLAutoDrawable) {
                val gl = drawable.gl
                
                // Process async events
                if (runLoop != null) {
                    runLoop.runOnce()
                }
                
                gl.glViewport(0, 0, drawable.surfaceWidth, drawable.surfaceHeight)
                if (frontend != null && map != null) {
                    try {
                        frontend.render()
                    } catch (e: Exception) {
                        println("Render error: ${e.message}")
                        e.printStackTrace()
                    }
                }
            }
            
            override fun reshape(drawable: GLAutoDrawable, x: Int, y: Int, width: Int, height: Int) {
                backend?.updateSize(width, height)
                map?.setSize(Size(width, height))
            }
            
            override fun dispose(drawable: GLAutoDrawable) {
                // TODO: Explicitly clean up memory (handled by Cleaner right now)
            }
        })
        
        val frame = JFrame("MapLibre JVM Demo")
        frame.addWindowListener(object : WindowAdapter() {
            override fun windowClosing(e: WindowEvent) {
                frame.dispose()
                exitProcess(0)
            }
        })
        
        frame.contentPane.add(glPanel, BorderLayout.CENTER)
        frame.setSize(800, 600)
        frame.setLocationRelativeTo(null)
        frame.isVisible = true
        
        Timer(100) {
            glPanel.repaint()
        }.apply {
            isRepeats = true
            start()
        }
        
        // Auto-close after 10 seconds (demo)
        Timer(10000) {
            frame.dispose()
            exitProcess(0)
        }.apply {
            isRepeats = false
            start()
        }
    }
}
