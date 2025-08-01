@file:Suppress("FunctionName")

package com.maplibre.jni

import kotlin.test.*

class ScreenCoordinateTest {
    @Test
    fun `create screen coordinate`() {
        ScreenCoordinate(100.0, 200.0).use { coord ->
            assertEquals(100.0, coord.x)
            assertEquals(200.0, coord.y)
        }
    }
    
    @Test
    fun `test screen coordinate equality`() {
        ScreenCoordinate(50.0, 75.0).use { coord1 ->
            ScreenCoordinate(50.0, 75.0).use { coord2 ->
                ScreenCoordinate(100.0, 150.0).use { coord3 ->
                    assertEquals(coord1, coord2)
                    assertNotEquals(coord1, coord3)
                }
            }
        }
    }
    
    @Test
    fun `test hashCode consistency`() {
        ScreenCoordinate(25.5, 30.5).use { coord1 ->
            ScreenCoordinate(25.5, 30.5).use { coord2 ->
                assertEquals(coord1.hashCode(), coord2.hashCode())
            }
        }
    }
    
    @Test
    fun `test toString format`() {
        ScreenCoordinate(123.45, 678.90).use { coord ->
            assertEquals("ScreenCoordinate(123.45, 678.9)", coord.toString())
        }
    }
    
    @Test
    fun `test negative coordinates`() {
        ScreenCoordinate(-50.0, -100.0).use { coord ->
            assertEquals(-50.0, coord.x)
            assertEquals(-100.0, coord.y)
        }
    }
    
    @Test
    fun `test zero coordinates`() {
        ScreenCoordinate(0.0, 0.0).use { coord ->
            assertEquals(0.0, coord.x)
            assertEquals(0.0, coord.y)
        }
    }
}