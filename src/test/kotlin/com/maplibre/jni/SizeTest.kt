@file:Suppress("FunctionName")

package com.maplibre.jni

import kotlin.test.Test
import kotlin.test.assertEquals
import kotlin.test.assertFalse
import kotlin.test.assertTrue

class SizeTest {
    @Test
    fun `create size with valid dimensions`() {
        Size(1920, 1080).use { size ->
            assertEquals(1920, size.width)
            assertEquals(1080, size.height)
            assertEquals(2073600, size.area)
            assertEquals(1.7777778f, size.aspectRatio)
            assertFalse(size.isEmpty)
        }
    }
    
    @Test
    fun `create empty size`() {
        Size(0, 0).use { size ->
            assertEquals(0, size.width)
            assertEquals(0, size.height)
            assertEquals(0, size.area)
            assertTrue(size.isEmpty)
        }
    }
    
    @Test
    fun `test size equality`() {
        Size(800, 600).use { size1 ->
            Size(800, 600).use { size2 ->
                Size(1024, 768).use { size3 ->
                    assertEquals(size1, size2)
                    assertFalse(size1 == size3)
                }
            }
        }
    }
    
    @Test
    fun `test size hashCode consistency`() {
        Size(640, 480).use { size1 ->
            Size(640, 480).use { size2 ->
                assertEquals(size1.hashCode(), size2.hashCode())
            }
        }
    }
    
    @Test
    fun `test toString format`() {
        Size(1024, 768).use { size ->
            assertEquals("Size(1024, 768)", size.toString())
        }
    }
}
