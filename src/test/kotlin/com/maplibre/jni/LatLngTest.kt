@file:Suppress("FunctionName")

package com.maplibre.jni

import kotlin.test.*

class LatLngTest {
    @Test
    fun `create latlng with valid coordinates`() {
        LatLng(40.7128, -74.0060).use { nyc ->
            assertEquals(40.7128, nyc.latitude)
            assertEquals(-74.0060, nyc.longitude)
        }
    }
    
    @Test
    fun `create latlng with wrapping`() {
        LatLng(35.6762, 239.6503, wrap = true).use { tokyo ->
            assertEquals(35.6762, tokyo.latitude)
            // When wrap = true, longitude is normalized to [-180, 180]
            // 239.6503 becomes -120.3497 (239.6503 - 360)
            assertEquals(-120.3497, tokyo.longitude, 0.0001)
        }
    }
    
    @Test
    fun `test wrapped method`() {
        LatLng(35.6762, 239.6503).use { tokyo ->
            assertEquals(239.6503, tokyo.longitude)
            
            val wrapped = tokyo.wrapped()
            wrapped.use {
                assertEquals(35.6762, wrapped.latitude)
                assertEquals(-120.34969999999998, wrapped.longitude, 0.00001)
            }
        }
    }
    
    @Test
    fun `test latlng equality`() {
        LatLng(51.5074, -0.1278).use { london1 ->
            LatLng(51.5074, -0.1278).use { london2 ->
                LatLng(48.8566, 2.3522).use { paris ->
                    assertEquals(london1, london2)
                    assertNotEquals(london1, paris)
                }
            }
        }
    }
    
    @Test
    fun `test hashCode consistency`() {
        LatLng(37.7749, -122.4194).use { sf1 ->
            LatLng(37.7749, -122.4194).use { sf2 ->
                assertEquals(sf1.hashCode(), sf2.hashCode())
            }
        }
    }
    
    @Test
    fun `test toString format`() {
        LatLng(52.5200, 13.4050).use { berlin ->
            assertEquals("LatLng(latitude=52.52, longitude=13.405)", berlin.toString())
        }
    }
    
    @Test
    fun `reject NaN latitude`() {
        assertFailsWith<IllegalArgumentException>("latitude must not be NaN") {
            LatLng(Double.NaN, 0.0)
        }
    }
    
    @Test
    fun `reject NaN longitude`() {
        assertFailsWith<IllegalArgumentException>("longitude must not be NaN") {
            LatLng(0.0, Double.NaN)
        }
    }
    
    @Test
    fun `reject latitude greater than 90`() {
        assertFailsWith<IllegalArgumentException>("latitude must be between -90 and 90") {
            LatLng(91.0, 0.0)
        }
    }
    
    @Test
    fun `reject latitude less than -90`() {
        assertFailsWith<IllegalArgumentException>("latitude must be between -90 and 90") {
            LatLng(-91.0, 0.0)
        }
    }
    
    @Test
    fun `reject infinite longitude`() {
        assertFailsWith<IllegalArgumentException>("longitude must not be infinite") {
            LatLng(0.0, Double.POSITIVE_INFINITY)
        }
    }
    
    @Test
    fun `test boundary values`() {
        // Test exact boundaries
        LatLng(90.0, 180.0).use { north ->
            assertEquals(90.0, north.latitude)
            assertEquals(180.0, north.longitude)
        }
        
        LatLng(-90.0, -180.0).use { south ->
            assertEquals(-90.0, south.latitude)
            assertEquals(-180.0, south.longitude)
        }
    }
    
    @Test
    fun `test wrap method`() {
        LatLng(35.6762, 239.6503).use { tokyo ->
            assertEquals(239.6503, tokyo.longitude)
            tokyo.wrap()
            // After wrap, longitude should be normalized to [-180, 180]
            assertEquals(-120.3497, tokyo.longitude, 0.0001)
        }
    }
    
    @Test
    fun `test unwrapForShortestPath`() {
        LatLng(0.0, -170.0).use { start ->
            LatLng(0.0, 170.0).use { end ->
                start.unwrapForShortestPath(end)
                // Should unwrap to take shortest path across dateline
                assertEquals(190.0, start.longitude, 0.0001)
            }
        }
    }
}
