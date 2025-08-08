package com.maplibre.jni

/**
 * Debug options for the map renderer.
 * Each property represents a debug feature that can be enabled.
 * Multiple options can be enabled simultaneously.
 */
@JvmInline
value class MapDebugOptions(val value: Int = 0) {
    
    constructor(
        tileBorders: Boolean = false,
        parseStatus: Boolean = false,
        timestamps: Boolean = false,
        collision: Boolean = false,
        overdraw: Boolean = false,
        stencilClip: Boolean = false,
        depthBuffer: Boolean = false
    ) : this(
        (if (tileBorders) TILE_BORDERS_FLAG else 0) or
        (if (parseStatus) PARSE_STATUS_FLAG else 0) or
        (if (timestamps) TIMESTAMPS_FLAG else 0) or
        (if (collision) COLLISION_FLAG else 0) or
        (if (overdraw) OVERDRAW_FLAG else 0) or
        (if (stencilClip) STENCIL_CLIP_FLAG else 0) or
        (if (depthBuffer) DEPTH_BUFFER_FLAG else 0)
    )
    
    val tileBorders: Boolean get() = (value and TILE_BORDERS_FLAG) != 0
    val parseStatus: Boolean get() = (value and PARSE_STATUS_FLAG) != 0
    val timestamps: Boolean get() = (value and TIMESTAMPS_FLAG) != 0
    val collision: Boolean get() = (value and COLLISION_FLAG) != 0
    val overdraw: Boolean get() = (value and OVERDRAW_FLAG) != 0
    val stencilClip: Boolean get() = (value and STENCIL_CLIP_FLAG) != 0
    val depthBuffer: Boolean get() = (value and DEPTH_BUFFER_FLAG) != 0
    
    val isNoDebug: Boolean get() = value == 0
    
    /**
     * Returns a copy of this MapDebugOptions with the specified options modified
     */
    fun copy(
        tileBorders: Boolean? = null,
        parseStatus: Boolean? = null,
        timestamps: Boolean? = null,
        collision: Boolean? = null,
        overdraw: Boolean? = null,
        stencilClip: Boolean? = null,
        depthBuffer: Boolean? = null
    ): MapDebugOptions {
        var newValue = value
        
        tileBorders?.let { 
            newValue = if (it) newValue or TILE_BORDERS_FLAG else newValue and TILE_BORDERS_FLAG.inv()
        }
        parseStatus?.let { 
            newValue = if (it) newValue or PARSE_STATUS_FLAG else newValue and PARSE_STATUS_FLAG.inv()
        }
        timestamps?.let { 
            newValue = if (it) newValue or TIMESTAMPS_FLAG else newValue and TIMESTAMPS_FLAG.inv()
        }
        collision?.let { 
            newValue = if (it) newValue or COLLISION_FLAG else newValue and COLLISION_FLAG.inv()
        }
        overdraw?.let { 
            newValue = if (it) newValue or OVERDRAW_FLAG else newValue and OVERDRAW_FLAG.inv()
        }
        stencilClip?.let { 
            newValue = if (it) newValue or STENCIL_CLIP_FLAG else newValue and STENCIL_CLIP_FLAG.inv()
        }
        depthBuffer?.let { 
            newValue = if (it) newValue or DEPTH_BUFFER_FLAG else newValue and DEPTH_BUFFER_FLAG.inv()
        }
        
        return MapDebugOptions(newValue)
    }
    
    /**
     * Combines this debug options with another
     */
    operator fun plus(other: MapDebugOptions): MapDebugOptions {
        return MapDebugOptions(value or other.value)
    }
    
    /**
     * Removes the specified debug options
     */
    operator fun minus(other: MapDebugOptions): MapDebugOptions {
        return MapDebugOptions(value and other.value.inv())
    }
    
    override fun toString(): String {
        if (isNoDebug) return "NoDebug"
        
        val options = mutableListOf<String>()
        if (tileBorders) options.add("TileBorders")
        if (parseStatus) options.add("ParseStatus")
        if (timestamps) options.add("Timestamps")
        if (collision) options.add("Collision")
        if (overdraw) options.add("Overdraw")
        if (stencilClip) options.add("StencilClip")
        if (depthBuffer) options.add("DepthBuffer")
        
        return options.joinToString(" | ")
    }
    
    companion object {
        // Bit flags for each debug option (using different names to avoid conflicts)
        private const val TILE_BORDERS_FLAG = 1 shl 1
        private const val PARSE_STATUS_FLAG = 1 shl 2
        private const val TIMESTAMPS_FLAG = 1 shl 3
        private const val COLLISION_FLAG = 1 shl 4
        private const val OVERDRAW_FLAG = 1 shl 5
        private const val STENCIL_CLIP_FLAG = 1 shl 6
        private const val DEPTH_BUFFER_FLAG = 1 shl 7
        
        // Predefined common combinations
        val NO_DEBUG = MapDebugOptions(0)
        val TILE_BORDERS = MapDebugOptions(tileBorders = true)
        val PARSE_STATUS = MapDebugOptions(parseStatus = true)
        val TIMESTAMPS = MapDebugOptions(timestamps = true)
        val COLLISION = MapDebugOptions(collision = true)
        val OVERDRAW = MapDebugOptions(overdraw = true)
        val STENCIL_CLIP = MapDebugOptions(stencilClip = true)
        val DEPTH_BUFFER = MapDebugOptions(depthBuffer = true)
    }
}