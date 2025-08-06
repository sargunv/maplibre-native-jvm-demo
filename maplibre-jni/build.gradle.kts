import org.gradle.internal.os.OperatingSystem

fun getPreset(): String {
    return project.findProperty("cmake.preset") as String? ?: when {
        OperatingSystem.current().isWindows -> {
            // Use Vulkan on Windows ARM64, WGL on x64
            if (System.getProperty("os.arch").contains("aarch64") || System.getProperty("os.arch").contains("arm64")) {
                "windows-vulkan"
            } else {
                "windows-wgl"
            }
        }
        OperatingSystem.current().isLinux -> "linux-egl"
        OperatingSystem.current().isMacOsX -> "macos-metal"
        else -> throw GradleException("Unsupported operating system")
    }
}

tasks.register<Exec>("configureCMake") {
    dependsOn(":generateKotlinMainJniHeaders")
    
    val preset = getPreset()

    // Use preset-specific subdirectory to avoid rebuilding when switching presets
    val buildDir = layout.buildDirectory.dir("cmake/${preset}").get().asFile
    val jniHeadersDir = layout.buildDirectory.dir("generated/jni-headers/kotlin/main").get().asFile
    
    inputs.file("CMakeLists.txt")
    inputs.dir("src/main/cpp")
    inputs.dir(jniHeadersDir)
    inputs.dir("../vendor/maplibre-native")
    
    outputs.dir(buildDir)
    outputs.file(buildDir.resolve("CMakeCache.txt"))
    
    doFirst {
        buildDir.mkdirs()
    }
    
    workingDir = buildDir

    commandLine(listOf(
        "cmake",
        "--preset",
        preset,
        projectDir.absolutePath
    ))
}

tasks.register<Exec>("build") {
    dependsOn("configureCMake")
    val preset = getPreset()
    
    val buildDir = layout.buildDirectory.dir("cmake/${preset}").get().asFile
    workingDir = buildDir
    
    inputs.files(fileTree("src/main/cpp"))
    inputs.dir(layout.buildDirectory.dir("generated/jni-headers/kotlin/main"))
    inputs.file(buildDir.resolve("CMakeCache.txt"))
    
    outputs.file(layout.buildDirectory.file("lib/main/shared/libmaplibre-jni.so"))
    outputs.dir(layout.buildDirectory.dir("lib"))
    
    commandLine("cmake", "--build", ".", "--config", "Release", "--parallel", Runtime.getRuntime().availableProcessors().toString())
}

tasks.register<Delete>("clean") {
    delete(layout.buildDirectory.dir("cmake"))
    delete(layout.buildDirectory.dir("lib"))
    delete(layout.buildDirectory.dir("vscode"))
}

tasks.withType<Test> {
    dependsOn("build")
    systemProperty("java.library.path", layout.buildDirectory.dir("lib/main/shared").get().asFile.absolutePath)
}
