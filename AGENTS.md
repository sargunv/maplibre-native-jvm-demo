# MapLibre Native JVM Demo - Agent Guidelines

## Project Overview
This project provides JVM bindings for MapLibre Native, enabling desktop map rendering in Java/Kotlin applications using AWT/Swing. The codebase consists of:
- **Kotlin API** (`src/main/kotlin/org/maplibre/kmp/native/`): High-level Kotlin classes for map interaction
- **JNI Bridge** (`maplibre-jni/`): C++ code that bridges Kotlin to MapLibre Native using JNI
- **Demo App** (`src/main/kotlin/`): Example Swing application demonstrating map features
- **Graphics Backends**: Supports OpenGL, Vulkan, and Metal rendering across Windows, Linux, and macOS

## Build & Test Commands
- **Build**: `./gradlew build` or `just build`
- **Run**: `./gradlew run` or `just run`
- **Run with backend**: `./gradlew run -Pcmake.preset=<preset>` (e.g., `linux-vulkan`, `macos-metal`)
- **Clean**: `./gradlew clean` or `just clean`
  - Rebuilding maplibre-native after a clean will take a long time. Avoid this as much as possible.

Not yet relevant to this project:
- **Test all**: `./gradlew test`
- **Test single**: `./gradlew test --tests "TestClassName"` or `./gradlew test --tests "TestClassName.methodName"`
- **Check**: `./gradlew check` (runs all verification tasks)

## Code Style Guidelines
- **Language**: Kotlin with JVM target 21
- **Imports**: Group by stdlib, java/javax, then project packages (org.maplibre.kmp.native)
- **Naming**: Use camelCase for functions/variables, PascalCase for classes/interfaces
- **Files**: One public class per file, filename matches class name
- **Nullability**: Prefer non-nullable types, use `?` suffix only when necessary
- **Collections**: Use Kotlin stdlib collections (List, Map, Set) over Java collections
- **Error handling**: Use Kotlin's Result type or exceptions, avoid null returns for errors
- **JNI**: Native methods use `external` keyword, follow ktjni plugin conventions
- **Testing**: Use Kotlin test framework (`kotlin("test")` dependency)
- **Documentation**: Use KDoc format for public APIs
