@file:Suppress("FunctionName")

package com.maplibre.jni

import kotlin.test.*

class CameraOptionsTest {
    @Test
    fun `create empty camera options`() {
        CameraOptions().use { options ->
            // Just ensure it doesn't crash
            assertNotNull(options)
        }
    }
    
    @Test
    fun `set center`() {
        CameraOptions().use { options ->
            LatLng(40.7128, -74.0060).use { nyc ->
                val result = options.withCenter(nyc)
                assertEquals(options, result) // Verify it returns self for chaining
            }
        }
    }
    
    @Test
    fun `set null center`() {
        CameraOptions().use { options ->
            val result = options.withCenter(null)
            assertEquals(options, result)
        }
    }
    
    @Test
    fun `set padding`() {
        CameraOptions().use { options ->
            EdgeInsets(10.0, 20.0, 30.0, 40.0).use { padding ->
                val result = options.withPadding(padding)
                assertEquals(options, result)
            }
        }
    }
    
    @Test
    fun `set null padding`() {
        CameraOptions().use { options ->
            val result = options.withPadding(null)
            assertEquals(options, result)
        }
    }
    
    @Test
    fun `set anchor`() {
        CameraOptions().use { options ->
            ScreenCoordinate(100.0, 200.0).use { anchor ->
                val result = options.withAnchor(anchor)
                assertEquals(options, result)
            }
        }
    }
    
    @Test
    fun `set null anchor`() {
        CameraOptions().use { options ->
            val result = options.withAnchor(null)
            assertEquals(options, result)
        }
    }
    
    @Test
    fun `set zoom`() {
        CameraOptions().use { options ->
            val result = options.withZoom(12.5)
            assertEquals(options, result)
        }
    }
    
    @Test
    fun `set null zoom`() {
        CameraOptions().use { options ->
            val result = options.withZoom(null)
            assertEquals(options, result)
        }
    }
    
    @Test
    fun `set bearing`() {
        CameraOptions().use { options ->
            val result = options.withBearing(45.0)
            assertEquals(options, result)
        }
    }
    
    @Test
    fun `set null bearing`() {
        CameraOptions().use { options ->
            val result = options.withBearing(null)
            assertEquals(options, result)
        }
    }
    
    @Test
    fun `set pitch`() {
        CameraOptions().use { options ->
            val result = options.withPitch(30.0)
            assertEquals(options, result)
        }
    }
    
    @Test
    fun `set null pitch`() {
        CameraOptions().use { options ->
            val result = options.withPitch(null)
            assertEquals(options, result)
        }
    }
    
    @Test
    fun `chain builder methods`() {
        CameraOptions().use { options ->
            LatLng(40.7128, -74.0060).use { nyc ->
                EdgeInsets(10.0, 20.0, 30.0, 40.0).use { padding ->
                    ScreenCoordinate(50.0, 50.0).use { anchor ->
                        val result = options
                            .withCenter(nyc)
                            .withZoom(12.5)
                            .withBearing(45.0)
                            .withPitch(30.0)
                            .withPadding(padding)
                            .withAnchor(anchor)
                        
                        assertEquals(options, result) // All methods should return the same instance
                    }
                }
            }
        }
    }
    
    @Test
    fun `reset fields to null`() {
        CameraOptions().use { options ->
            LatLng(40.7128, -74.0060).use { nyc ->
                // Set values
                options
                    .withCenter(nyc)
                    .withZoom(12.5)
                    .withBearing(45.0)
                    .withPitch(30.0)
                
                // Verify values are set
                assertNotNull(options.center)
                assertEquals(12.5, options.zoom)
                assertEquals(45.0, options.bearing)
                assertEquals(30.0, options.pitch)
                
                // Reset to null
                options
                    .withCenter(null)
                    .withZoom(null)
                    .withBearing(null)
                    .withPitch(null)
                
                // Verify all are null
                assertNull(options.center)
                assertNull(options.zoom)
                assertNull(options.bearing)
                assertNull(options.pitch)
            }
        }
    }
    
    @Test
    fun `set extreme numeric values`() {
        CameraOptions().use { options ->
            // Test edge cases for numeric values
            options
                .withZoom(0.0)      // Minimum zoom
                .withZoom(22.0)     // Maximum typical zoom
                .withBearing(0.0)   // North
                .withBearing(359.9) // Almost full rotation
                .withPitch(0.0)     // Flat
                .withPitch(60.0)    // Maximum typical pitch
            
            // Verify the last set values
            assertEquals(22.0, options.zoom)
            assertEquals(359.9, options.bearing)
            assertEquals(60.0, options.pitch)
        }
    }
    
    @Test
    fun `test getters with all fields set`() {
        CameraOptions().use { options ->
            val center = LatLng(37.7749, -122.4194)
            val padding = EdgeInsets(10.0, 20.0, 30.0, 40.0)
            val anchor = ScreenCoordinate(100.0, 200.0)
            
            options
                .withCenter(center)
                .withPadding(padding)
                .withAnchor(anchor)
                .withZoom(15.0)
                .withBearing(45.0)
                .withPitch(60.0)
            
            // Verify all values are retrieved correctly
            assertNotNull(options.center)
            assertEquals(37.7749, options.center!!.latitude, 0.0001)
            assertEquals(-122.4194, options.center!!.longitude, 0.0001)
            
            assertNotNull(options.padding)
            assertEquals(10.0, options.padding!!.top, 0.0001)
            assertEquals(20.0, options.padding!!.left, 0.0001)
            assertEquals(30.0, options.padding!!.bottom, 0.0001)
            assertEquals(40.0, options.padding!!.right, 0.0001)
            
            assertNotNull(options.anchor)
            assertEquals(100.0, options.anchor!!.x, 0.0001)
            assertEquals(200.0, options.anchor!!.y, 0.0001)
            
            assertEquals(15.0, options.zoom)
            assertEquals(45.0, options.bearing)
            assertEquals(60.0, options.pitch)
            
            // Clean up the created objects from getters
            options.center?.close()
            options.padding?.close()
            options.anchor?.close()
            
            center.close()
            padding.close()
            anchor.close()
        }
    }
    
    @Test
    fun `test hashCode consistency`() {
        CameraOptions().use { options1 ->
            CameraOptions().use { options2 ->
                val center = LatLng(37.7749, -122.4194)
                val padding = EdgeInsets(10.0, 20.0, 30.0, 40.0)
                
                // Both with same values
                options1
                    .withCenter(center)
                    .withPadding(padding)
                    .withZoom(15.0)
                    .withBearing(45.0)
                
                options2
                    .withCenter(center)
                    .withPadding(padding)
                    .withZoom(15.0)
                    .withBearing(45.0)
                
                // Should have same hashCode for equal objects
                assertEquals(options1, options2)
                assertEquals(options1.hashCode(), options2.hashCode())
                
                // Clean up
                center.close()
                padding.close()
            }
        }
    }
    
    @Test
    fun `test hashCode with null values`() {
        CameraOptions().use { options1 ->
            CameraOptions().use { options2 ->
                // Both empty should have same hashCode
                assertEquals(options1.hashCode(), options2.hashCode())
                
                // Set one field
                options1.withZoom(10.0)
                assertNotEquals(options1.hashCode(), options2.hashCode())
            }
        }
    }
}