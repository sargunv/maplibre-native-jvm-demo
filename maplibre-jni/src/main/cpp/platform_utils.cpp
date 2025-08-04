#include <jni.h>
#include "jni_helpers.hpp"

#ifdef __APPLE__
#import <AppKit/AppKit.h>
#elif defined(__linux__)
#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include <cstdlib>
#include <cstring>
#endif

extern "C" {

JNIEXPORT jfloat JNICALL Java_com_maplibre_jni_PlatformUtils_getSystemPixelRatio
  (JNIEnv* env, jclass) {
    
#ifdef __APPLE__
    // On macOS, use NSScreen's backingScaleFactor
    @autoreleasepool {
        NSScreen* screen = [NSScreen mainScreen];
        if (screen) {
            return static_cast<jfloat>(screen.backingScaleFactor);
        }
        return 1.0f;
    }
    
#elif defined(__linux__)
    // On Linux, read Xft.dpi from X resources
    Display* display = XOpenDisplay(nullptr);
    if (!display) {
        return 1.0f;
    }
    
    float pixelRatio = 1.0f;
    
    char* resourceString = XResourceManagerString(display);
    if (resourceString) {
        XrmInitialize();
        XrmDatabase db = XrmGetStringDatabase(resourceString);
        
        if (db) {
            XrmValue value;
            char* type = nullptr;
            
            // Try to get Xft.dpi resource
            if (XrmGetResource(db, "Xft.dpi", "Xft.Dpi", &type, &value) == True) {
                if (value.addr) {
                    float dpi = static_cast<float>(atof(value.addr));
                    // Standard DPI is 96, so scale factor = dpi / 96
                    if (dpi > 0) {
                        pixelRatio = dpi / 96.0f;
                    }
                }
            }
            
            XrmDestroyDatabase(db);
        }
    }
    
    XCloseDisplay(display);
    return pixelRatio;
    
#elif defined(_WIN32)
    // Windows support not yet implemented
    // TODO: Use GetDpiForWindow() or GetDpiForMonitor()
    return 1.0f;
    
#else
    // Unknown platform
    return 1.0f;
#endif
}

} // extern "C"