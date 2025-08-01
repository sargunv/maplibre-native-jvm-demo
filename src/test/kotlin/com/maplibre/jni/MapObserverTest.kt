@file:Suppress("FunctionName")

package com.maplibre.jni

import kotlin.test.*

class MapObserverTest {
    @Test
    fun `create native observer wrapper`() {
        val kotlinObserver = object : MapObserver {
            var mapLoadedCalled = false
            var frameRenderedCount = 0
            
            override fun onDidFinishLoadingMap() {
                mapLoadedCalled = true
            }
            
            override fun onDidFinishRenderingFrame(status: MapObserver.RenderFrameStatus) {
                frameRenderedCount++
            }
        }
        
        NativeMapObserver(kotlinObserver).use { nativeObserver ->
            assertNotNull(nativeObserver)
            // The native observer is created and ready to forward callbacks
            // Actual callback testing would require a Map instance
        }
    }
    
    @Test
    fun `test enum values`() {
        // Verify enums are accessible
        assertEquals(2, MapObserver.CameraChangeMode.values().size)
        assertEquals(2, MapObserver.RenderMode.values().size)
        assertEquals(4, MapLoadError.values().size)
        
        // Verify enum names match what native expects
        assertEquals("IMMEDIATE", MapObserver.CameraChangeMode.IMMEDIATE.name)
        assertEquals("ANIMATED", MapObserver.CameraChangeMode.ANIMATED.name)
        assertEquals("PARTIAL", MapObserver.RenderMode.PARTIAL.name)
        assertEquals("FULL", MapObserver.RenderMode.FULL.name)
    }
    
    @Test
    fun `test render frame status data class`() {
        val status = MapObserver.RenderFrameStatus(
            mode = MapObserver.RenderMode.FULL,
            needsRepaint = true,
            placementChanged = false
        )
        
        assertEquals(MapObserver.RenderMode.FULL, status.mode)
        assertTrue(status.needsRepaint)
        assertFalse(status.placementChanged)
    }
    
    @Test
    fun `test default observer implementation`() {
        // Verify all methods have default implementations
        val observer = object : MapObserver {}
        
        // Should not throw - all methods have defaults
        observer.onCameraWillChange(MapObserver.CameraChangeMode.IMMEDIATE)
        observer.onCameraIsChanging()
        observer.onCameraDidChange(MapObserver.CameraChangeMode.ANIMATED)
        observer.onWillStartLoadingMap()
        observer.onDidFinishLoadingMap()
        observer.onDidFailLoadingMap(MapLoadError.STYLE_PARSE_ERROR, "test error")
        observer.onWillStartRenderingFrame()
        observer.onDidFinishRenderingFrame(
            MapObserver.RenderFrameStatus(
                MapObserver.RenderMode.PARTIAL, 
                false, 
                true
            )
        )
        observer.onWillStartRenderingMap()
        observer.onDidFinishRenderingMap(MapObserver.RenderMode.FULL)
        observer.onDidFinishLoadingStyle()
        observer.onStyleImageMissing("missing-image")
        observer.onDidBecomeIdle()
    }
}