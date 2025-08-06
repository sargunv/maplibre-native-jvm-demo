# Development workflow for MapLibre Native JVM bindings

# Use Git Bash on Windows, default bash on other platforms
set windows-shell := ["C:/Program Files/Git/usr/bin/bash.exe", "-c"]

# Default recipe - show available commands
default:
    @just --list

# Build and run the Kotlin application
run:
    ./gradlew run

# Clean all build artifacts
clean: clean-vcpkg
    ./gradlew clean

# Build everything
build:
    ./gradlew build

# Reset the vcpkg submodule to a clean state
clean-vcpkg:
    cd vendor/maplibre-native/platform/windows/vendor/vcpkg && git reset --hard && git clean -fdx
