package com.maplibre.jni

import java.awt.Component
import java.awt.event.*
import kotlin.math.abs

/**
 * Handles user interactions (mouse and keyboard) for a MapLibre map.
 * This is an optional component that can be attached to a map to enable
 * pan, zoom, rotate, and tilt interactions.
 *
 * Mouse controls:
 * - Left drag: Pan the map
 * - Mouse wheel: Zoom in/out
 * - Right drag horizontal: Rotate the map
 * - Right drag vertical: Tilt/pitch the map
 * - Ctrl+Left drag: Alternative for rotate/tilt
 * - Double-click: Zoom in at point (animated)
 * - Shift+Double-click: Zoom out at point (animated)
 *
 * Keyboard controls:
 * - Arrow keys: Pan the map
 * - +/- or =/- keys: Zoom in/out
 * - N: Reset north orientation
 */
class MapInteractionHandler(
    private val component: Component,
    private val map: MaplibreMap,
    private val config: Config = Config()
) : MouseListener, MouseMotionListener, MouseWheelListener, KeyListener {

    data class Config(
        val enablePan: Boolean = true,
        val enableZoom: Boolean = true,
        val enableRotate: Boolean = true,
        val enableTilt: Boolean = true,
        val enableKeyboard: Boolean = true,
        val panSpeed: Double = 1.0,
        val zoomSpeed: Double = 1.0,
        val rotateSpeed: Double = 1.0,
        val tiltSpeed: Double = 1.0
    )

    private var enabled = false

    // Gesture tracking
    private var isPanning = false
    private var isRotatingAndTilting = false  // Combined right-click gesture

    // Mouse position tracking
    private var lastX = 0.0
    private var lastY = 0.0
    
    // Initial position for rotation anchor
    private var rotationAnchorX = 0.0
    private var rotationAnchorY = 0.0

    // Double-click detection
    private var lastClickTime = 0L
    private val doubleClickThreshold = 400L // milliseconds

    // Get pixel ratio for proper scaling
    private val pixelRatio: Float
        get() = (component as? java.awt.Canvas)?.graphicsConfiguration?.defaultTransform?.scaleX?.toFloat() ?: 1.0f

    /**
     * Enable interaction handling
     */
    fun enable() {
        if (enabled) return

        component.addMouseListener(this)
        component.addMouseMotionListener(this)
        component.addMouseWheelListener(this)
        if (config.enableKeyboard) {
            component.addKeyListener(this)
            component.isFocusable = true
        }

        enabled = true
    }

    /**
     * Disable interaction handling
     */
    fun disable() {
        if (!enabled) return

        component.removeMouseListener(this)
        component.removeMouseMotionListener(this)
        component.removeMouseWheelListener(this)
        if (config.enableKeyboard) {
            component.removeKeyListener(this)
        }

        enabled = false
    }

    // MouseListener implementation
    override fun mouseClicked(e: MouseEvent) {
        if (!enabled) return

        // Only handle double-click for primary mouse button
        if (e.button != MouseEvent.BUTTON1) return

        val currentTime = System.currentTimeMillis()
        val timeSinceLastClick = currentTime - lastClickTime

        if (timeSinceLastClick < doubleClickThreshold) {
            // Double-click detected
            handleDoubleClick(e)
        }

        lastClickTime = currentTime
    }

    override fun mousePressed(e: MouseEvent) {
        if (!enabled) return

        lastX = e.x.toDouble()
        lastY = e.y.toDouble()

        when {
            // Right-click or Ctrl+Left click for rotation and tilt
            e.button == MouseEvent.BUTTON3 ||
            (e.button == MouseEvent.BUTTON1 && e.isControlDown) -> {
                if (config.enableRotate || config.enableTilt) {
                    isRotatingAndTilting = true
                    map.setGestureInProgress(true)
                    // Store the anchor point for rotation
                    rotationAnchorX = e.x.toDouble()
                    rotationAnchorY = e.y.toDouble()
                }
            }
            // Left click for pan
            e.button == MouseEvent.BUTTON1 -> {
                if (config.enablePan) {
                    isPanning = true
                    map.setGestureInProgress(true)
                }
            }
        }

        // Request focus for keyboard input
        if (!component.hasFocus()) {
            component.requestFocusInWindow()
        }
    }

    override fun mouseReleased(e: MouseEvent) {
        if (!enabled) return

        if (isPanning || isRotatingAndTilting) {
            map.setGestureInProgress(false)
        }

        isPanning = false
        isRotatingAndTilting = false
    }

    override fun mouseEntered(e: MouseEvent) {}
    override fun mouseExited(e: MouseEvent) {}

    // MouseMotionListener implementation
    override fun mouseDragged(e: MouseEvent) {
        if (!enabled) return

        val dx = e.x - lastX
        val dy = e.y - lastY

        when {
            isPanning && config.enablePan -> {
                // Pan the map - scale by pixel ratio for proper speed
                map.moveBy(ScreenCoordinate(
                    dx * config.panSpeed * pixelRatio,
                    dy * config.panSpeed * pixelRatio
                ))
            }
            isRotatingAndTilting -> {
                // Right-click drag: X for rotation, Y for pitch
                // Both rotate around the initial click point

                if (config.enableRotate && Math.abs(dx) > 0.01) {
                    // Rotate around the anchor point where we clicked
                    val currentCamera = map.getCameraOptions()
                    val currentBearing = currentCamera.bearing ?: 0.0
                    val bearingDelta = dx * config.rotateSpeed
                    val anchor = ScreenCoordinate(
                        rotationAnchorX * pixelRatio,
                        rotationAnchorY * pixelRatio
                    )
                    
                    // Use jumpTo with anchor to rotate around the click point
                    map.jumpTo(
                        CameraOptions(
                            center = null,  // Must be null to use anchor
                            bearing = currentBearing + bearingDelta,
                            zoom = currentCamera.zoom,
                            pitch = currentCamera.pitch,
                            anchor = anchor
                        )
                    )
                }

                if (config.enableTilt && Math.abs(dy) > 0.01) {
                    // Tilt based on vertical movement
                    map.pitchBy(dy * config.tiltSpeed)
                }
            }
        }

        lastX = e.x.toDouble()
        lastY = e.y.toDouble()
    }

    override fun mouseMoved(e: MouseEvent) {
        lastX = e.x.toDouble()
        lastY = e.y.toDouble()
    }

    // MouseWheelListener implementation
    override fun mouseWheelMoved(e: MouseWheelEvent) {
        if (!enabled || !config.enableZoom) return

        val rotation = e.wheelRotation
        val scrollAmount = e.scrollAmount.toDouble()

        // Calculate zoom factor - positive rotation for natural scrolling (scroll up = zoom in)
        val delta = rotation * scrollAmount * 10
        val absDelta = abs(delta)
        var scale = 2.0 / (1.0 + Math.exp(-absDelta / 100.0))

        // Make scroll wheel a bit slower
        scale = (scale - 1.0) / 2.0 + 1.0

        // Zoom out if scrolling down (positive rotation)
        if (delta > 0) {
            scale = 1.0 / scale
        }

        // Apply zoom with speed adjustment - scale anchor by pixel ratio
        val adjustedScale = 1.0 + (scale - 1.0) * config.zoomSpeed
        map.scaleBy(
            adjustedScale,
            ScreenCoordinate(e.x.toDouble() * pixelRatio, e.y.toDouble() * pixelRatio)
        )
    }

    // KeyListener implementation
    override fun keyPressed(e: KeyEvent) {
        if (!enabled || !config.enableKeyboard) return

        when (e.keyCode) {
            // Pan with arrow keys - scale by pixel ratio
            KeyEvent.VK_LEFT -> {
                if (config.enablePan) {
                    map.moveBy(ScreenCoordinate(50.0 * config.panSpeed * pixelRatio, 0.0))
                }
            }
            KeyEvent.VK_RIGHT -> {
                if (config.enablePan) {
                    map.moveBy(ScreenCoordinate(-50.0 * config.panSpeed * pixelRatio, 0.0))
                }
            }
            KeyEvent.VK_UP -> {
                if (config.enablePan) {
                    map.moveBy(ScreenCoordinate(0.0, 50.0 * config.panSpeed * pixelRatio))
                }
            }
            KeyEvent.VK_DOWN -> {
                if (config.enablePan) {
                    map.moveBy(ScreenCoordinate(0.0, -50.0 * config.panSpeed * pixelRatio))
                }
            }
            // Zoom with +/- keys - animated like double-click
            KeyEvent.VK_PLUS, KeyEvent.VK_EQUALS -> {
                if (config.enableZoom) {
                    val currentCamera = map.getCameraOptions()
                    val currentZoom = currentCamera.zoom ?: 0.0
                    map.easeTo(currentCamera.copy(zoom = currentZoom + 1.0), 300)
                }
            }
            KeyEvent.VK_MINUS -> {
                if (config.enableZoom) {
                    val currentCamera = map.getCameraOptions()
                    val currentZoom = currentCamera.zoom ?: 0.0
                    map.easeTo(currentCamera.copy(zoom = currentZoom - 1.0), 300)
                }
            }
            // Reset north with N
            KeyEvent.VK_N -> {
                val current = map.getCameraOptions()
                map.easeTo(current.copy(bearing = 0.0), 300)
            }
        }
    }

    override fun keyReleased(e: KeyEvent) {}
    override fun keyTyped(e: KeyEvent) {}

    // Private helper methods
    private fun handleDoubleClick(e: MouseEvent) {
        if (!config.enableZoom) return

        // Get current zoom and calculate new zoom
        val currentCamera = map.getCameraOptions()
        val currentZoom = currentCamera.zoom ?: 0.0
        val newZoom = if (e.isShiftDown) {
            currentZoom - 1.0  // Zoom out
        } else {
            currentZoom + 1.0  // Zoom in
        }

        // Use the click position as the anchor point for zooming
        // This keeps the clicked location fixed during the zoom animation
        val anchor = ScreenCoordinate(
            e.x.toDouble() * pixelRatio,
            e.y.toDouble() * pixelRatio
        )

        // Animate zoom with anchor point
        // Note: center and anchor are mutually exclusive in MapLibre
        // We must clear the center to use the anchor
        map.easeTo(
            CameraOptions(
                center = null,  // Must be null to use anchor
                zoom = newZoom,
                bearing = currentCamera.bearing,
                pitch = currentCamera.pitch,
                anchor = anchor
            ),
            300
        )
    }
}
