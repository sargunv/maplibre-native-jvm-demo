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
    
    from("maplibre-jni/build/lib/main/shared/$libName")
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
