import com.maplibre.jni.*
import java.awt.Component
import java.awt.event.KeyEvent
import java.awt.event.KeyListener

/**
 * Demo-specific keyboard handler that adds additional shortcuts for the demo application.
 * Extends the base MapInteractionHandler to add demo features like style switching.
 *
 * Additional keyboard shortcuts:
 * - S: Cycle through available map styles
 * - X: Reset transform to default view (San Francisco)
 * - A: Fly through predefined locations
 * - D: Cycle through debug options
 * - R: Toggle rendering statistics view
 */
class DemoKeyboardHandler(
    private val component: Component,
    private val map: MaplibreMap,
    config: MapInteractionHandler.Config = MapInteractionHandler.Config()
) : KeyListener {
    
    // Delegate basic interaction to the library handler
    private val baseHandler = MapInteractionHandler(component, map, config)
    
    // Demo-specific state
    private var currentStyleIndex = DemoStyle.DEFAULT.ordinal
    private var locationIndex = 0
    private var currentDebugIndex = 0
    
    // Debug options in cycle order (matching GLFW demo)
    private val debugCycle = listOf(
        MapDebugOptions.NO_DEBUG,
        MapDebugOptions.TILE_BORDERS,
        MapDebugOptions.PARSE_STATUS,
        MapDebugOptions.TIMESTAMPS,
        MapDebugOptions.COLLISION,
        MapDebugOptions.OVERDRAW
    )
    
    // Predefined locations for fly-through
    private val locations = listOf(
        Location("San Francisco", LatLng(37.7749, -122.4194), 13.0),
        Location("New York", LatLng(40.7128, -74.0060), 13.0),
        Location("London", LatLng(51.5074, -0.1278), 13.0),
        Location("Tokyo", LatLng(35.6762, 139.6503), 13.0),
        Location("Sydney", LatLng(-33.8688, 151.2093), 13.0),
        Location("Paris", LatLng(48.8566, 2.3522), 13.0),
        Location("Berlin", LatLng(52.5200, 13.4050), 13.0),
        Location("Moscow", LatLng(55.7558, 37.6173), 13.0),
        Location("Beijing", LatLng(39.9042, 116.4074), 13.0),
        Location("Rio de Janeiro", LatLng(-22.9068, -43.1729), 13.0)
    )
    
    data class Location(
        val name: String,
        val latLng: LatLng,
        val zoom: Double
    )
    
    fun enable() {
        baseHandler.enable()
        // Add our own key listener on top
        component.addKeyListener(this)
    }
    
    fun disable() {
        baseHandler.disable()
        component.removeKeyListener(this)
    }
    
    override fun keyPressed(e: KeyEvent) {
        // Let base handler process first
        baseHandler.keyPressed(e)
        
        // Handle demo-specific keys
        when (e.keyCode) {
            KeyEvent.VK_S -> {
                // Cycle through styles
                currentStyleIndex = (currentStyleIndex + 1) % DemoStyle.entries.size
                val style = DemoStyle.getByIndex(currentStyleIndex)
                println("Switching to style: ${style.styleName}")
                map.loadStyleURL(style.url)
            }
            
            KeyEvent.VK_X -> {
                // Reset transform to default San Francisco view
                println("Resetting to default view")
                map.easeTo(
                    CameraOptions(
                        center = LatLng(37.7749, -122.4194),
                        zoom = 13.0,
                        bearing = 0.0,
                        pitch = 0.0
                    ),
                    500 // Animation duration in milliseconds
                )
            }
            
            KeyEvent.VK_A -> {
                // Fly through predefined locations
                val location = locations[locationIndex]
                println("Flying to: ${location.name}")
                map.flyTo(
                    CameraOptions(
                        center = location.latLng,
                        zoom = location.zoom,
                        bearing = 0.0,
                        pitch = 0.0
                    )
                )
                locationIndex = (locationIndex + 1) % locations.size
            }
            
            KeyEvent.VK_D -> {
                // Cycle through debug options
                currentDebugIndex = (currentDebugIndex + 1) % debugCycle.size
                val debugOption = debugCycle[currentDebugIndex]
                map.setDebug(debugOption)
                println("Debug mode: $debugOption")
            }
            
            KeyEvent.VK_R -> {
                // Toggle rendering statistics view
                val currentlyEnabled = map.isRenderingStatsViewEnabled()
                map.enableRenderingStatsView(!currentlyEnabled)
                println("Rendering stats: ${if (!currentlyEnabled) "ON" else "OFF"}")
            }
        }
    }
    
    override fun keyReleased(e: KeyEvent) {
        baseHandler.keyReleased(e)
    }
    
    override fun keyTyped(e: KeyEvent) {
        baseHandler.keyTyped(e)
    }
}