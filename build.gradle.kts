import org.gradle.internal.os.OperatingSystem

plugins {
    kotlin("jvm") version "2.2.0"
    application
    id("io.github.fletchmckee.ktjni") version "0.1.0"
}

repositories {
    mavenCentral()
}

dependencies {
    kotlin("stdlib")
    
    testImplementation(kotlin("test"))
}

kotlin {
    jvmToolchain(21)
}

application {
    mainClass.set("MainKt")
}

ktjni {
    outputDir = project(":maplibre-jni").layout.buildDirectory.dir("generated/jni-headers").get().asFile
}

// Copy the built library to resources
tasks.register<Copy>("copyNativeLibrary") {
    dependsOn(":maplibre-jni:build")
    
    val os = OperatingSystem.current()
    val libName = when {
        os.isLinux -> "libmaplibre-jni.so"
        os.isMacOsX -> "libmaplibre-jni.dylib"
        os.isWindows -> "maplibre-jni.dll"
        else -> throw GradleException("Unsupported OS")
    }
    
    val osFolder = when {
        os.isLinux -> "linux"
        os.isMacOsX -> "macos"
        os.isWindows -> "windows"
        else -> throw GradleException("Unsupported OS")
    }
    
    // Copy all libraries from the shared folder
    from("maplibre-jni/build/lib/main/shared/") {
        include("*.dll", "*.so", "*.dylib")
    }
    into("$buildDir/generated/resources/main/native/$osFolder")
}

sourceSets {
    main {
        resources {
            srcDir("$buildDir/generated/resources/main")
        }
    }
    test {
        resources {
            srcDir("$buildDir/generated/resources/main")
        }
    }
}

tasks.named("processResources") {
    dependsOn("copyNativeLibrary")
}

tasks.named("processTestResources") {
    dependsOn("copyNativeLibrary")
}

// Configure run task to include Homebrew library paths on macOS
tasks.named<JavaExec>("run") {
    if (OperatingSystem.current().isMacOsX) {
        // Detect Homebrew path based on architecture
        val homebrewPath = if (System.getProperty("os.arch") == "aarch64") {
            "/opt/homebrew"
        } else {
            "/usr/local"
        }
        
        // Use DYLD_FALLBACK_LIBRARY_PATH to avoid conflicts with system libraries
        val existingPath = System.getenv("DYLD_FALLBACK_LIBRARY_PATH") ?: "/usr/local/lib:/usr/lib"
        val vulkanSdkPath = System.getenv("VULKAN_SDK")?.let { "$it/lib" } ?: ""
        
        val paths = listOf(
            "$homebrewPath/lib",
            vulkanSdkPath,
            existingPath
        ).filter { it.isNotEmpty() }.joinToString(":")
        
        environment("DYLD_FALLBACK_LIBRARY_PATH", paths)
    }
}
