@file:Suppress("FunctionName")

package com.maplibre.jni

import kotlin.test.*

class MapOptionsTest {
    @Test
    fun `create default map options`() {
        MapOptions().use { options ->
            assertNotNull(options)
            // Check default values
            assertEquals(MapMode.CONTINUOUS, options.mapMode)
            assertEquals(ConstrainMode.HEIGHT_ONLY, options.constrainMode)
            assertEquals(ViewportMode.DEFAULT, options.viewportMode)
            assertTrue(options.crossSourceCollisions) // Default is true
            assertEquals(NorthOrientation.UPWARDS, options.northOrientation)
            assertEquals(64, options.size.width) // Default size
            assertEquals(64, options.size.height)
            assertEquals(1.0f, options.pixelRatio)
            
            // Clean up size from getter
            options.size.close()
        }
    }
    
    @Test
    fun `set map mode`() {
        MapOptions().use { options ->
            val result = options.withMapMode(MapMode.STATIC)
            assertEquals(options, result) // Verify chaining
            assertEquals(MapMode.STATIC, options.mapMode)
            
            options.withMapMode(MapMode.TILE)
            assertEquals(MapMode.TILE, options.mapMode)
        }
    }
    
    @Test
    fun `set constrain mode`() {
        MapOptions().use { options ->
            val result = options.withConstrainMode(ConstrainMode.WIDTH_AND_HEIGHT)
            assertEquals(options, result)
            assertEquals(ConstrainMode.WIDTH_AND_HEIGHT, options.constrainMode)
            
            options.withConstrainMode(ConstrainMode.NONE)
            assertEquals(ConstrainMode.NONE, options.constrainMode)
        }
    }
    
    @Test
    fun `set viewport mode`() {
        MapOptions().use { options ->
            val result = options.withViewportMode(ViewportMode.FLIPPED_Y)
            assertEquals(options, result)
            assertEquals(ViewportMode.FLIPPED_Y, options.viewportMode)
        }
    }
    
    @Test
    fun `set cross source collisions`() {
        MapOptions().use { options ->
            val result = options.withCrossSourceCollisions(false)
            assertEquals(options, result)
            assertFalse(options.crossSourceCollisions)
            
            options.withCrossSourceCollisions(true)
            assertTrue(options.crossSourceCollisions)
        }
    }
    
    @Test
    fun `set north orientation`() {
        MapOptions().use { options ->
            val result = options.withNorthOrientation(NorthOrientation.RIGHTWARDS)
            assertEquals(options, result)
            assertEquals(NorthOrientation.RIGHTWARDS, options.northOrientation)
            
            // Test all orientations
            NorthOrientation.values().forEach { orientation ->
                options.withNorthOrientation(orientation)
                assertEquals(orientation, options.northOrientation)
            }
        }
    }
    
    @Test
    fun `set size`() {
        MapOptions().use { options ->
            Size(800, 600).use { size ->
                val result = options.withSize(size)
                assertEquals(options, result)
                
                options.size.use { retrievedSize ->
                    assertEquals(800, retrievedSize.width)
                    assertEquals(600, retrievedSize.height)
                }
            }
        }
    }
    
    @Test
    fun `set pixel ratio`() {
        MapOptions().use { options ->
            val result = options.withPixelRatio(2.0f)
            assertEquals(options, result)
            assertEquals(2.0f, options.pixelRatio)
        }
    }
    
    @Test
    fun `reject invalid pixel ratio`() {
        MapOptions().use { options ->
            assertFailsWith<IllegalArgumentException> {
                options.withPixelRatio(0.0f)
            }
            assertFailsWith<IllegalArgumentException> {
                options.withPixelRatio(-1.0f)
            }
        }
    }
    
    @Test
    fun `chain builder methods`() {
        MapOptions().use { options ->
            Size(1024, 768).use { size ->
                val result = options
                    .withMapMode(MapMode.STATIC)
                    .withConstrainMode(ConstrainMode.SCREEN)
                    .withViewportMode(ViewportMode.FLIPPED_Y)
                    .withCrossSourceCollisions(false)
                    .withNorthOrientation(NorthOrientation.DOWNWARDS)
                    .withSize(size)
                    .withPixelRatio(1.5f)
                
                assertEquals(options, result)
                assertEquals(MapMode.STATIC, options.mapMode)
                assertEquals(ConstrainMode.SCREEN, options.constrainMode)
                assertEquals(ViewportMode.FLIPPED_Y, options.viewportMode)
                assertFalse(options.crossSourceCollisions)
                assertEquals(NorthOrientation.DOWNWARDS, options.northOrientation)
                assertEquals(1.5f, options.pixelRatio)
                
                options.size.use { retrievedSize ->
                    assertEquals(1024, retrievedSize.width)
                    assertEquals(768, retrievedSize.height)
                }
            }
        }
    }
    
    @Test
    fun `test equals and hashCode`() {
        MapOptions().use { options1 ->
            MapOptions().use { options2 ->
                Size(512, 512).use { size ->
                    // Initially equal (both default)
                    assertEquals(options1, options2)
                    assertEquals(options1.hashCode(), options2.hashCode())
                    
                    // Clean up default sizes
                    options1.size.close()
                    options2.size.close()
                    
                    // Set same values
                    options1.withMapMode(MapMode.TILE).withPixelRatio(2.0f).withSize(size)
                    options2.withMapMode(MapMode.TILE).withPixelRatio(2.0f).withSize(size)
                    
                    // Clean up new sizes
                    val size1 = options1.size
                    val size2 = options2.size
                    
                    assertEquals(options1, options2)
                    assertEquals(options1.hashCode(), options2.hashCode())
                    
                    // Different values
                    options1.withMapMode(MapMode.CONTINUOUS)
                    assertNotEquals(options1, options2)
                    assertNotEquals(options1.hashCode(), options2.hashCode())
                    
                    size1.close()
                    size2.close()
                }
            }
        }
    }
    
    @Test
    fun `test toString`() {
        MapOptions().use { options ->
            val str = options.toString()
            assertTrue(str.contains("MapOptions"))
            assertTrue(str.contains("mapMode="))
            assertTrue(str.contains("constrainMode="))
            assertTrue(str.contains("pixelRatio="))
            
            options.size.close()
        }
    }
}