#include "jni_helpers.hpp"
#include "surface_descriptors.hpp"
#include <jni.h>
#include <jawt.h>
#include <jawt_md.h>

#if defined(__APPLE__)
#import <Cocoa/Cocoa.h>
#import <QuartzCore/CAMetalLayer.h>
#endif

using namespace maplibre_jni;

extern "C" {

JNIEXPORT jlong JNICALL Java_org_maplibre_kmp_native_internal_Native_extractMetalLayerSurface(JNIEnv* env, jclass, jobject canvas, jint width, jint height) {
#if defined(__APPLE__)
    JAWT awt; awt.version = JAWT_VERSION_9;
    if (JAWT_GetAWT(env, &awt) == JNI_FALSE) return 0;
    JAWT_DrawingSurface* ds = awt.GetDrawingSurface(env, canvas);
    if (!ds) return 0;
    jint lock = ds->Lock(ds);
    if ((lock & JAWT_LOCK_ERROR) != 0) { awt.FreeDrawingSurface(ds); return 0; }
    JAWT_DrawingSurfaceInfo* dsi = ds->GetDrawingSurfaceInfo(ds);
    if (!dsi) { ds->Unlock(ds); awt.FreeDrawingSurface(ds); return 0; }

    id<JAWT_SurfaceLayers> surfaceLayers = (id<JAWT_SurfaceLayers>)dsi->platformInfo;
    if (!surfaceLayers) { ds->FreeDrawingSurfaceInfo(dsi); ds->Unlock(ds); awt.FreeDrawingSurface(ds); return 0; }

    CAMetalLayer* layer = (CAMetalLayer*)surfaceLayers.layer;
    if (!layer) {
        layer = [CAMetalLayer layer];
        surfaceLayers.layer = layer;
    }

    NSScreen* screen = [NSScreen mainScreen];
    CGFloat scale = screen.backingScaleFactor;
    layer.contentsScale = scale;
    layer.frame = CGRectMake(0, 0, width / scale, height / scale);

    ds->FreeDrawingSurfaceInfo(dsi);
    ds->Unlock(ds);
    awt.FreeDrawingSurface(ds);

    auto* desc = new SurfaceDescriptor();
    desc->kind = SurfaceKind::Mac;
    desc->u.mac.caLayer = (void*)layer;
    return toJavaPointer(desc);
#else
    (void)canvas; (void)width; (void)height; return 0;
#endif
}

JNIEXPORT void JNICALL Java_org_maplibre_kmp_native_internal_Native_destroySurfaceDescriptor(JNIEnv* env, jclass, jlong surfacePtr) {
    delete fromJavaPointer<SurfaceDescriptor>(surfacePtr);
}

}
