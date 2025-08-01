# Development workflow for MapLibre Native JVM bindings

# Default recipe - show available commands
default:
    @just --list

# Build and run the Kotlin application
run:
    ./gradlew run

# Clean all build artifacts
clean:
    ./gradlew clean

# Build everything
build:
    ./gradlew build

# Generate JNI headers from Kotlin classes
generate-headers:
    # Use ktjni plugin to generate JNI headers
    rm -rf maplibre-jni/src/main/cpp/generated
    ./gradlew :maplibre-jni:generateJniHeaders
    @echo "Generated headers:"
    @ls -la maplibre-jni/src/main/cpp/generated/

symlink-compile-commands:
    ln -s ./maplibre-jni/build/cmake/compile_commands.json compile_commands.json
