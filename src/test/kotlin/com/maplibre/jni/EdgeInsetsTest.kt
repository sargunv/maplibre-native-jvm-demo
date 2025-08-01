package com.maplibre.jni

import com.maplibre.jni.EdgeInsets
import kotlin.test.*

class EdgeInsetsTest {
    @Test
    fun `create edge insets with valid values`() {
        val insets = EdgeInsets(10.0, 20.0, 30.0, 40.0)
        assertEquals(10.0, insets.top)
        assertEquals(20.0, insets.left)
        assertEquals(30.0, insets.bottom)
        assertEquals(40.0, insets.right)
        assertFalse(insets.isFlush)
    }

    @Test
    fun `create edge insets with default values`() {
        val insets = EdgeInsets()
        assertEquals(0.0, insets.top)
        assertEquals(0.0, insets.left)
        assertEquals(0.0, insets.bottom)
        assertEquals(0.0, insets.right)
        assertTrue(insets.isFlush)
    }

    @Test
    fun `test edge insets equality`() {
        val insets1 = EdgeInsets(10.0, 20.0, 30.0, 40.0)
        val insets2 = EdgeInsets(10.0, 20.0, 30.0, 40.0)
        val insets3 = EdgeInsets(10.0, 20.0, 30.0, 50.0)
        
        assertEquals(insets1, insets2)
        assertNotEquals(insets1, insets3)
    }

    @Test
    fun `test hashCode consistency`() {
        val insets1 = EdgeInsets(10.0, 20.0, 30.0, 40.0)
        val insets2 = EdgeInsets(10.0, 20.0, 30.0, 40.0)
        
        assertEquals(insets1.hashCode(), insets2.hashCode())
    }

    @Test
    fun `test toString format`() {
        val insets = EdgeInsets(10.5, 20.5, 30.5, 40.5)
        assertEquals("EdgeInsets(top=10.5, left=20.5, bottom=30.5, right=40.5)", insets.toString())
    }

    @Test
    fun `test plus operator`() {
        val insets1 = EdgeInsets(10.0, 20.0, 30.0, 40.0)
        val insets2 = EdgeInsets(5.0, 10.0, 15.0, 20.0)
        val result = insets1 + insets2
        
        assertEquals(15.0, result.top)
        assertEquals(30.0, result.left)
        assertEquals(45.0, result.bottom)
        assertEquals(60.0, result.right)
    }

    @Test
    fun `test negative values`() {
        val insets = EdgeInsets(-10.0, -20.0, -30.0, -40.0)
        assertEquals(-10.0, insets.top)
        assertEquals(-20.0, insets.left)
        assertEquals(-30.0, insets.bottom)
        assertEquals(-40.0, insets.right)
        assertFalse(insets.isFlush)
    }

    @Test
    fun `validate NaN is rejected`() {
        assertFailsWith<IllegalArgumentException> {
            EdgeInsets(Double.NaN, 0.0, 0.0, 0.0)
        }
        assertFailsWith<IllegalArgumentException> {
            EdgeInsets(0.0, Double.NaN, 0.0, 0.0)
        }
        assertFailsWith<IllegalArgumentException> {
            EdgeInsets(0.0, 0.0, Double.NaN, 0.0)
        }
        assertFailsWith<IllegalArgumentException> {
            EdgeInsets(0.0, 0.0, 0.0, Double.NaN)
        }
    }

    @Test
    fun `test infinity values are allowed`() {
        val insets = EdgeInsets(Double.POSITIVE_INFINITY, Double.NEGATIVE_INFINITY, 0.0, 0.0)
        assertEquals(Double.POSITIVE_INFINITY, insets.top)
        assertEquals(Double.NEGATIVE_INFINITY, insets.left)
    }

    @Test
    fun `test partial constructor usage`() {
        val insets1 = EdgeInsets(top = 10.0)
        assertEquals(10.0, insets1.top)
        assertEquals(0.0, insets1.left)
        assertEquals(0.0, insets1.bottom)
        assertEquals(0.0, insets1.right)

        val insets2 = EdgeInsets(left = 20.0, right = 40.0)
        assertEquals(0.0, insets2.top)
        assertEquals(20.0, insets2.left)
        assertEquals(0.0, insets2.bottom)
        assertEquals(40.0, insets2.right)
    }
    
    @Test
    fun `test plusAssign operator`() {
        EdgeInsets(10.0, 20.0, 30.0, 40.0).use { insets1 ->
            EdgeInsets(5.0, 10.0, 15.0, 20.0).use { insets2 ->
                insets1 += insets2
                assertEquals(15.0, insets1.top)
                assertEquals(30.0, insets1.left)
                assertEquals(45.0, insets1.bottom)
                assertEquals(60.0, insets1.right)
            }
        }
    }
    
    @Test
    fun `test getCenter`() {
        EdgeInsets(10.0, 20.0, 30.0, 40.0).use { insets ->
            // With a 200x200 viewport, center should account for insets
            // Available width: 200 - 20 - 40 = 140
            // Available height: 200 - 10 - 30 = 160
            // Center x: 20 + 140/2 = 90
            // Center y: 10 + 160/2 = 90
            val center = insets.getCenter(200, 200)
            center.use {
                assertEquals(90.0, center.x)
                assertEquals(90.0, center.y)
            }
        }
    }
}
