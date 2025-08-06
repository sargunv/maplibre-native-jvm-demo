import org.gradle.internal.os.OperatingSystem

tasks.register<Exec>("configureCMake") {
    dependsOn(":generateKotlinMainJniHeaders")
    
    val buildDir = layout.buildDirectory.dir("cmake").get().asFile
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
    
    val preset = when {
        OperatingSystem.current().isWindows -> "windows-opengl"
        OperatingSystem.current().isLinux -> "linux-opengl"
        OperatingSystem.current().isMacOsX -> "macos-metal"
        else -> throw GradleException("Unsupported operating system")
    }

    commandLine(listOf(
        "cmake",
        "--preset",
        preset,
        projectDir.absolutePath
    ))
}

tasks.register<Exec>("build") {
    dependsOn("configureCMake")
    
    val buildDir = layout.buildDirectory.dir("cmake").get().asFile
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
    doLast {
        project.exec {
            commandLine("git", "submodule", "foreach", "--recursive", "git reset --hard && git clean -xfd")
        }
    }
}

tasks.withType<Test> {
    dependsOn("build")
    systemProperty("java.library.path", layout.buildDirectory.dir("lib/main/shared").get().asFile.absolutePath)
}
