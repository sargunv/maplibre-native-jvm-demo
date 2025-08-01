@file:Suppress("FunctionName")

package com.maplibre.jni

import kotlin.test.*

class ResourceOptionsTest {
    @Test
    fun `create default resource options`() {
        ResourceOptions().use { options ->
            assertNotNull(options)
            // Default values
            assertEquals("", options.apiKey)
            assertEquals(":memory:", options.cachePath) // Default is in-memory cache
            assertEquals(".", options.assetPath) // Default is current directory
            assertTrue(options.maximumCacheSize > 0) // Should have a sensible default
        }
    }
    
    @Test
    fun `set api key`() {
        ResourceOptions().use { options ->
            val apiKey = "test-api-key-12345"
            val result = options.withApiKey(apiKey)
            assertEquals(options, result) // Verify chaining
            assertEquals(apiKey, options.apiKey)
        }
    }
    
    @Test
    fun `set cache path`() {
        ResourceOptions().use { options ->
            val cachePath = "/tmp/maplibre-cache"
            val result = options.withCachePath(cachePath)
            assertEquals(options, result)
            assertEquals(cachePath, options.cachePath)
        }
    }
    
    @Test
    fun `set asset path`() {
        ResourceOptions().use { options ->
            val assetPath = "/assets/maplibre"
            val result = options.withAssetPath(assetPath)
            assertEquals(options, result)
            assertEquals(assetPath, options.assetPath)
        }
    }
    
    @Test
    fun `set maximum cache size`() {
        ResourceOptions().use { options ->
            val cacheSize = 100L * 1024L * 1024L // 100 MB
            val result = options.withMaximumCacheSize(cacheSize)
            assertEquals(options, result)
            assertEquals(cacheSize, options.maximumCacheSize)
        }
    }
    
    @Test
    fun `reject negative cache size`() {
        ResourceOptions().use { options ->
            assertFailsWith<IllegalArgumentException> {
                options.withMaximumCacheSize(-1)
            }
        }
    }
    
    @Test
    fun `chain builder methods`() {
        ResourceOptions().use { options ->
            val result = options
                .withApiKey("test-key")
                .withCachePath("/cache")
                .withAssetPath("/assets")
                .withMaximumCacheSize(50 * 1024 * 1024)
            
            assertEquals(options, result)
            assertEquals("test-key", options.apiKey)
            assertEquals("/cache", options.cachePath)
            assertEquals("/assets", options.assetPath)
            assertEquals(50 * 1024 * 1024, options.maximumCacheSize)
        }
    }
    
    @Test
    fun `clone resource options`() {
        ResourceOptions().use { original ->
            original
                .withApiKey("original-key")
                .withCachePath("/original/cache")
                .withAssetPath("/original/assets")
                .withMaximumCacheSize(200 * 1024 * 1024)
            
            original.clone().use { cloned ->
                // Verify cloned has same values
                assertEquals(original.apiKey, cloned.apiKey)
                assertEquals(original.cachePath, cloned.cachePath)
                assertEquals(original.assetPath, cloned.assetPath)
                assertEquals(original.maximumCacheSize, cloned.maximumCacheSize)
                
                // Verify they are different objects
                assertNotSame(original, cloned)
                
                // Verify changes to cloned don't affect original
                cloned.withApiKey("cloned-key")
                assertEquals("original-key", original.apiKey)
                assertEquals("cloned-key", cloned.apiKey)
            }
        }
    }
    
    @Test
    fun `test equals and hashCode`() {
        ResourceOptions().use { options1 ->
            ResourceOptions().use { options2 ->
                // Initially equal (both default)
                assertEquals(options1, options2)
                assertEquals(options1.hashCode(), options2.hashCode())
                
                // Set same values
                options1.withApiKey("key").withCachePath("/cache")
                options2.withApiKey("key").withCachePath("/cache")
                assertEquals(options1, options2)
                assertEquals(options1.hashCode(), options2.hashCode())
                
                // Different values
                options1.withApiKey("different-key")
                assertNotEquals(options1, options2)
                assertNotEquals(options1.hashCode(), options2.hashCode())
            }
        }
    }
    
    @Test
    fun `test toString`() {
        ResourceOptions().use { options ->
            options
                .withApiKey("test-key")
                .withCachePath("/cache/path")
                .withAssetPath("/asset/path")
                .withMaximumCacheSize(1024)
            
            val str = options.toString()
            assertTrue(str.contains("test-key"))
            assertTrue(str.contains("/cache/path"))
            assertTrue(str.contains("/asset/path"))
            assertTrue(str.contains("1024"))
        }
    }
    
    // TODO: Test withTileServerOptions when TileServerOptions wrapper is implemented
    // TODO: Test withPlatformContext when platform-specific handling is implemented
}