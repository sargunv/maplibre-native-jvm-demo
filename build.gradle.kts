import org.gradle.internal.os.OperatingSystem

plugins {
    kotlin("jvm") version "2.2.0"
    application
    id("io.github.fletchmckee.ktjni") version "0.1.0"
}

repositories {
    mavenCentral()
    maven {
        url = uri("https://jogamp.org/deployment/maven/")
    }
}

dependencies {
    kotlin("stdlib")
    implementation("org.jogamp.gluegen:gluegen-rt:2.5.0")
    implementation("org.jogamp.jogl:jogl-all:2.5.0")
    implementation("org.jogamp.gluegen:gluegen-rt:2.5.0:natives-linux-amd64")
    implementation("org.jogamp.jogl:jogl-all:2.5.0:natives-linux-amd64")
    
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
    
    from("maplibre-jni/build/lib/main/shared/$libName")
    into("$buildDir/generated/resources/main/native/${os.familyName}")
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
    dependsOn("copyNativeLibrary")
}

tasks.named("processTestResources") {
    dependsOn("copyNativeLibrary")
}
