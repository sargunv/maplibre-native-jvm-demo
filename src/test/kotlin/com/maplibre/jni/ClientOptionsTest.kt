@file:Suppress("FunctionName")

package com.maplibre.jni

import kotlin.test.*

class ClientOptionsTest {
    @Test
    fun `create default client options`() {
        ClientOptions().use { options ->
            assertNotNull(options)
            // Default values should be empty strings
            assertEquals("", options.name)
            assertEquals("", options.version)
        }
    }
    
    @Test
    fun `set name`() {
        ClientOptions().use { options ->
            val name = "MyMapApp"
            val result = options.withName(name)
            assertEquals(options, result) // Verify chaining
            assertEquals(name, options.name)
        }
    }
    
    @Test
    fun `set version`() {
        ClientOptions().use { options ->
            val version = "1.2.3"
            val result = options.withVersion(version)
            assertEquals(options, result)
            assertEquals(version, options.version)
        }
    }
    
    @Test
    fun `chain builder methods`() {
        ClientOptions().use { options ->
            val result = options
                .withName("TestApp")
                .withVersion("2.0.0")
            
            assertEquals(options, result)
            assertEquals("TestApp", options.name)
            assertEquals("2.0.0", options.version)
        }
    }
    
    @Test
    fun `clone client options`() {
        ClientOptions().use { original ->
            original
                .withName("OriginalApp")
                .withVersion("1.0.0")
            
            original.clone().use { cloned ->
                // Verify cloned has same values
                assertEquals(original.name, cloned.name)
                assertEquals(original.version, cloned.version)
                
                // Verify they are different objects
                assertNotSame(original, cloned)
                
                // Verify changes to cloned don't affect original
                cloned.withName("ClonedApp")
                assertEquals("OriginalApp", original.name)
                assertEquals("ClonedApp", cloned.name)
            }
        }
    }
    
    @Test
    fun `test equals and hashCode`() {
        ClientOptions().use { options1 ->
            ClientOptions().use { options2 ->
                // Initially equal (both default)
                assertEquals(options1, options2)
                assertEquals(options1.hashCode(), options2.hashCode())
                
                // Set same values
                options1.withName("App").withVersion("1.0")
                options2.withName("App").withVersion("1.0")
                assertEquals(options1, options2)
                assertEquals(options1.hashCode(), options2.hashCode())
                
                // Different name
                options1.withName("DifferentApp")
                assertNotEquals(options1, options2)
                assertNotEquals(options1.hashCode(), options2.hashCode())
                
                // Same name, different version
                options1.withName("App")
                options2.withVersion("2.0")
                assertNotEquals(options1, options2)
                assertNotEquals(options1.hashCode(), options2.hashCode())
            }
        }
    }
    
    @Test
    fun `test toString`() {
        ClientOptions().use { options ->
            options
                .withName("MapLibreApp")
                .withVersion("3.1.4")
            
            val str = options.toString()
            assertTrue(str.contains("MapLibreApp"))
            assertTrue(str.contains("3.1.4"))
        }
    }
    
    @Test
    fun `handle empty strings`() {
        ClientOptions().use { options ->
            options
                .withName("")
                .withVersion("")
            
            assertEquals("", options.name)
            assertEquals("", options.version)
        }
    }
    
    @Test
    fun `handle special characters`() {
        ClientOptions().use { options ->
            options
                .withName("App with spaces & symbols!")
                .withVersion("1.0.0-beta+build.123")
            
            assertEquals("App with spaces & symbols!", options.name)
            assertEquals("1.0.0-beta+build.123", options.version)
        }
    }
}