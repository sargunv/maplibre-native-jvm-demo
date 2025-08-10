#pragma once

#include <cstdint>

namespace maplibre_jni {

enum class SurfaceKind : uint32_t {
    Mac,    // CAMetalLayer*
    Win32,  // HWND + HDC
    X11     // Display* + Window/Drawable
};

struct MacSurface { void* caLayer; };
struct Win32Surface { void* hwnd; void* hdc; };
struct X11Surface { void* display; unsigned long window; };

struct SurfaceDescriptor {
    SurfaceKind kind;
    union {
        MacSurface mac;
        Win32Surface win;
        X11Surface x11;
    } u;
};

} // namespace maplibre_jni
