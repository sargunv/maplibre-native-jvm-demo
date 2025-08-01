import org.gradle.internal.os.OperatingSystem

// Task to configure CMake
tasks.register<Exec>("configureCMake") {
    dependsOn(":generateKotlinMainJniHeaders")
    
    val buildDir = layout.buildDirectory.dir("cmake").get().asFile
    
    // Inputs
    inputs.file("CMakeLists.txt")
    inputs.dir("src/main/cpp")
    // Headers are already in src/main/cpp/generated, included via inputs.dir("src/main/cpp")
    inputs.dir("../vendor/maplibre-native")
    
    // Outputs
    outputs.dir(buildDir)
    outputs.file(buildDir.resolve("CMakeCache.txt"))
    
    doFirst {
        buildDir.mkdirs()
    }
    
    workingDir = buildDir
    
    val generator = if (OperatingSystem.current().isWindows) {
        "Visual Studio 17 2022"
    } else {
        "Ninja"
    }
    
    commandLine("cmake",
        "-G", generator,
        "-DCMAKE_BUILD_TYPE=Release",
        "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON",
        projectDir.absolutePath
    )
}

tasks.register<Exec>("buildNative") {
    dependsOn("configureCMake")
    
    val buildDir = layout.buildDirectory.dir("cmake").get().asFile
    workingDir = buildDir
    
    // Inputs
    inputs.files(fileTree("src/main/cpp"))
    // Headers are included via src/main/cpp input above
    inputs.file(buildDir.resolve("CMakeCache.txt"))
    
    // Outputs
    outputs.file(layout.buildDirectory.file("lib/main/shared/libmaplibre-jni.so"))
    outputs.dir(layout.buildDirectory.dir("lib"))
    
    commandLine("cmake", "--build", ".", "--config", "Release", "--parallel", Runtime.getRuntime().availableProcessors().toString())
}

// Clean task for CMake build
tasks.register<Delete>("clean") {
    delete(layout.buildDirectory.dir("cmake"))
    delete(layout.buildDirectory.dir("lib"))
}

// Create build task that depends on native build
tasks.register("build") {
    dependsOn("buildNative")
}

// Configure library path for tests
tasks.withType<Test> {
    dependsOn("buildNative")
    systemProperty("java.library.path", layout.buildDirectory.dir("lib/main/shared").get().asFile.absolutePath)
}