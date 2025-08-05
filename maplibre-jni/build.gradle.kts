import org.gradle.internal.os.OperatingSystem

// Task to configure CMake
tasks.register<Exec>("configureCMake") {
    dependsOn(":generateKotlinMainJniHeaders")
    
    val buildDir = layout.buildDirectory.dir("cmake").get().asFile
    val jniHeadersDir = layout.buildDirectory.dir("generated/jni-headers/kotlin/main").get().asFile
    
    // Inputs
    inputs.file("CMakeLists.txt")
    inputs.dir("src/main/cpp")
    inputs.dir(jniHeadersDir)
    inputs.dir("../vendor/maplibre-native")
    
    // Outputs
    outputs.dir(buildDir)
    outputs.file(buildDir.resolve("CMakeCache.txt"))
    
    doFirst {
        buildDir.mkdirs()
    }
    
    workingDir = buildDir
    
    val vcpkgToolchain = file("../vendor/maplibre-native/platform/windows/vendor/vcpkg/scripts/buildsystems/vcpkg.cmake")
    
    val cmakeArgs = mutableListOf(
        "cmake",
        "-G", "Ninja",
        "-DCMAKE_BUILD_TYPE=Release",
        "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON",
        "-DJNI_HEADERS_DIR=${jniHeadersDir.absolutePath}"
    )
    
    if (OperatingSystem.current().isWindows) {
        cmakeArgs.add("-DCMAKE_TOOLCHAIN_FILE=${vcpkgToolchain.absolutePath}")
    }
    
    cmakeArgs.add(projectDir.absolutePath)
    
    commandLine(cmakeArgs)
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
}

tasks.withType<Test> {
    dependsOn("build")
    systemProperty("java.library.path", layout.buildDirectory.dir("lib/main/shared").get().asFile.absolutePath)
}
