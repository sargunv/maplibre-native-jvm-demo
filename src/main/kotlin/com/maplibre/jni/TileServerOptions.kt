package com.maplibre.jni

/**
 * Configuration for tile server endpoints and URL templates.
 * This configures how MapLibre connects to map data providers.
 */
data class TileServerOptions(
    val baseURL: String = "https://api.mapbox.com",
    val uriSchemeAlias: String = "mapbox://",
    val apiKeyParameterName: String = "access_token",
    val requiresApiKey: Boolean = false,

    // Source templates
    val sourceTemplate: String = "https://api.mapbox.com",
    val sourceDomainName: String = "",
    val sourceVersionPrefix: String? = null,

    // Style templates
    val styleTemplate: String = "/styles/v1/{user}/{style}",
    val styleDomainName: String = "",
    val styleVersionPrefix: String? = null,

    // Sprites templates
    val spritesTemplate: String = "/styles/v1/{user}/{style}/sprite",
    val spritesDomainName: String = "",
    val spritesVersionPrefix: String? = null,

    // Glyphs templates
    val glyphsTemplate: String = "/fonts/v1/{user}/{fontstack}/{range}.pbf",
    val glyphsDomainName: String = "",
    val glyphsVersionPrefix: String? = null,

    // Tile templates
    val tileTemplate: String = "/v4/{tileset}/{z}/{x}/{y}{ratio}.{format}",
    val tileDomainName: String = "",
    val tileVersionPrefix: String? = null,

    // Default style
    val defaultStyle: String = ""
) {
    companion object {
      val DemoTiles =  TileServerOptions(
          baseURL = "https://demotiles.maplibre.org",
          uriSchemeAlias = "maplibre://",
          apiKeyParameterName = "",
          requiresApiKey = false,
          sourceTemplate = "https://demotiles.maplibre.org",
          styleTemplate = "/style.json",
          spritesTemplate = "/sprite",
          glyphsTemplate = "/fonts/{fontstack}/{range}.pbf",
          tileTemplate = "/tiles/{z}/{x}/{y}.pbf"
        )

        val Mapbox = TileServerOptions(
            baseURL = "https://api.mapbox.com",
            uriSchemeAlias = "mapbox://",
            apiKeyParameterName = "access_token",
            requiresApiKey = true,
            sourceTemplate = "https://api.mapbox.com",
            styleTemplate = "/styles/v1/{user}/{style}",
            spritesTemplate = "/styles/v1/{user}/{style}/sprite",
            glyphsTemplate = "/fonts/v1/{user}/{fontstack}/{range}.pbf",
            tileTemplate = "/v4/{tileset}/{z}/{x}/{y}{ratio}.{format}"
        )

        val MapTiler = TileServerOptions(
            baseURL = "https://api.maptiler.com",
            uriSchemeAlias = "maptiler://",
            apiKeyParameterName = "key",
            requiresApiKey = true,
            sourceTemplate = "https://api.maptiler.com",
            styleTemplate = "/maps/{style}/style.json",
            spritesTemplate = "/maps/{style}/sprite",
            glyphsTemplate = "/fonts/{fontstack}/{range}.pbf",
            tileTemplate = "/tiles/{tileset}/{z}/{x}/{y}.pbf"
        )
    }
}
