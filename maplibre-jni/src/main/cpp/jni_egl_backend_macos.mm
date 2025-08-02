#ifdef __APPLE__

#include "jni_egl_backend.hpp"
#include <mbgl/util/logging.hpp>
#import <Cocoa/Cocoa.h>
#import <QuartzCore/CALayer.h>
#include <jawt.h>
#include <jawt_md.h>
#include <sstream>

// Define JAWT_VERSION_9 if not available
#ifndef JAWT_VERSION_9
#define JAWT_VERSION_9 0x00090000
#endif

namespace maplibre_jni {

void* EGLRendererBackend::getNativeWindowHandleMacOS(JNIEnv* env, jobject canvas) {
    // Get JAWT
    JAWT awt;
    awt.version = JAWT_VERSION_9;  // Use version 9 for modern Java
    
    // Get the JAWT interface
    jboolean result = JAWT_GetAWT(env, &awt);
    if (result == JNI_FALSE) {
        mbgl::Log::Error(mbgl::Event::OpenGL, "JAWT_GetAWT failed");
        return nullptr;
    }
    
    // Get the drawing surface
    JAWT_DrawingSurface* ds = awt.GetDrawingSurface(env, canvas);
    if (!ds) {
        mbgl::Log::Error(mbgl::Event::OpenGL, "GetDrawingSurface returned null");
        return nullptr;
    }
    
    // Lock the drawing surface
    jint lock = ds->Lock(ds);
    if ((lock & JAWT_LOCK_ERROR) != 0) {
        mbgl::Log::Error(mbgl::Event::OpenGL, "Failed to lock drawing surface");
        awt.FreeDrawingSurface(ds);
        return nullptr;
    }
    
    // Get the drawing surface info
    JAWT_DrawingSurfaceInfo* dsi = ds->GetDrawingSurfaceInfo(ds);
    if (!dsi) {
        mbgl::Log::Error(mbgl::Event::OpenGL, "GetDrawingSurfaceInfo returned null");
        ds->Unlock(ds);
        awt.FreeDrawingSurface(ds);
        return nullptr;
    }
    
    // Get the platform-specific info
    id<JAWT_SurfaceLayers> surfaceLayers = (id<JAWT_SurfaceLayers>)dsi->platformInfo;
    if (!surfaceLayers) {
        mbgl::Log::Error(mbgl::Event::OpenGL, "platformInfo is null");
        ds->FreeDrawingSurfaceInfo(dsi);
        ds->Unlock(ds);
        awt.FreeDrawingSurface(ds);
        return nullptr;
    }
    
    // Get the bounds from the drawing surface info
    CGRect bounds = CGRectMake(dsi->bounds.x, dsi->bounds.y, 
                               dsi->bounds.width, dsi->bounds.height);
    
    // Get the CALayer
    CALayer* layer = surfaceLayers.layer;
    if (!layer) {
        // Try to get the window layer if the direct layer is not available
        layer = surfaceLayers.windowLayer;
        if (!layer) {
            // If no layer exists, we need to create one and set it
            layer = [CALayer layer];
            
            // Configure the layer for OpenGL ES rendering
            layer.opaque = YES;
            layer.contentsScale = [[NSScreen mainScreen] backingScaleFactor];
            layer.bounds = bounds;
            layer.anchorPoint = CGPointMake(0, 0);
            layer.position = CGPointMake(0, 0);
            
            // Set the layer on the surface
            surfaceLayers.layer = layer;
            
            std::stringstream msg;
            msg << "Created new CALayer for rendering with bounds: " 
                << bounds.size.width << "x" << bounds.size.height 
                << ", scale: " << layer.contentsScale;
            mbgl::Log::Info(mbgl::Event::OpenGL, msg.str());
        } else {
            mbgl::Log::Info(mbgl::Event::OpenGL, "Using window CALayer");
            // Update the layer bounds to match the canvas
            layer.bounds = bounds;
        }
    } else {
        mbgl::Log::Info(mbgl::Event::OpenGL, "Using existing CALayer");
        // Update the layer bounds to match the canvas
        layer.bounds = bounds;
    }
    
    // Store the JAWT structures for later cleanup
    // Note: In a real implementation, we'd need to properly manage these
    jawtDrawingSurface = ds;
    jawtDrawingSurfaceInfo = dsi;
    
    // Return the CALayer as the native window handle
    // ANGLE's Metal backend expects a CALayer
    return (__bridge void*)layer;
}

void EGLRendererBackend::releaseNativeWindowHandleMacOS() {
    if (jawtDrawingSurfaceInfo && jawtDrawingSurface) {
        JAWT_DrawingSurface* ds = (JAWT_DrawingSurface*)jawtDrawingSurface;
        ds->FreeDrawingSurfaceInfo((JAWT_DrawingSurfaceInfo*)jawtDrawingSurfaceInfo);
        ds->Unlock(ds);
        
        // Get JAWT to free the surface
        JAWT awt;
        awt.version = JAWT_VERSION_9;
        JNIEnv* env = getEnv();
        if (env && JAWT_GetAWT(env, &awt) != JNI_FALSE) {
            awt.FreeDrawingSurface(ds);
        }
        
        jawtDrawingSurface = nullptr;
        jawtDrawingSurfaceInfo = nullptr;
    }
}

} // namespace maplibre_jni

#endif // __APPLE__