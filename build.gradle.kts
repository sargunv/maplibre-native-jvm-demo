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

// Configure ktjni plugin
ktjni {
    outputDir = file("maplibre-jni/src/main/cpp/generated")
}

// Copy the built library to resources
tasks.register<Copy>("copyNativeLibrary") {
    dependsOn(":maplibre-jni:buildNative")
    
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
    
    from("maplibre-jni/build/lib/main/shared/$libName")
    into("$buildDir/generated/resources/main/native/$osFolder")
}

// Copy ANGLE libraries for macOS (now copied by CMake)
tasks.register<Copy>("copyAngleLibraries") {
    dependsOn(":maplibre-jni:buildNative")
    val os = OperatingSystem.current()
    
    if (os.isMacOsX) {
        // ANGLE libraries are now downloaded and copied by CMake
        from("maplibre-jni/build/lib/main/shared/libEGL.dylib")
        from("maplibre-jni/build/lib/main/shared/libGLESv2.dylib")
        into("$buildDir/generated/resources/main/native/macos")
    } else if (os.isWindows) {
        // TODO: Add Windows ANGLE libraries when available
        from("libs/windows/libEGL.dll")
        from("libs/windows/libGLESv2.dll")
        into("$buildDir/generated/resources/main/native/windows")
    }
    // Linux can use system EGL or bundled ANGLE
}

// Add the generated resources to the source set
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
    dependsOn("copyNativeLibrary", "copyAngleLibraries")
}

tasks.named("processTestResources") {
    dependsOn("copyNativeLibrary", "copyAngleLibraries")
}
