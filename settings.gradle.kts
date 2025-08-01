rootProject.name = "kotlin-maplibre-native"

pluginManagement {
    repositories {
        mavenCentral() // Release versions
        maven {
            url = uri("https://central.sonatype.com/repository/maven-snapshots/") // Snapshot versions
        }
        gradlePluginPortal()
    }
}

include("maplibre-jni")