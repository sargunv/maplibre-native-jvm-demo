#include "org_maplibre_kmp_native_MaplibreMap.h"
#include "jni_helpers.hpp"
// #include "awt_canvas_renderer.hpp"
#include "jni_renderer_frontend.hpp"
#include "map_observer.hpp"

//#if defined(USE_EGL_BACKEND) || defined(USE_WGL_BACKEND)
//#include "awt_gl_backend.hpp"
//#endif
#include "conversions/size_conversions.hpp"
#include "conversions/cameraoptions_conversions.hpp"
#include "conversions/mapoptions_conversions.hpp"
#include "conversions/clientoptions_conversions.hpp"
#include "conversions/resourceoptions_conversions.hpp"
#include "conversions/screencoordinate_conversions.hpp"
#include "conversions/latlng_conversions.hpp"
#include <mbgl/map/map.hpp>
#include <mbgl/map/map_options.hpp>
#include <mbgl/map/mode.hpp>
#include <mbgl/storage/resource_options.hpp>
#include <mbgl/util/client_options.hpp>
#include <mbgl/map/camera.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/util/size.hpp>
#include <mbgl/annotation/annotation.hpp>
#include <mbgl/storage/file_source_manager.hpp>
#include <mbgl/storage/database_file_source.hpp>
#include <memory>

// Wrapper struct to manage objects whose lifetime must match the Map's lifetime

struct MapWrapperWithFrontend {
    std::unique_ptr<mbgl::Map> map;
    std::unique_ptr<maplibre_jni::JniMapObserver> observer;
    std::unique_ptr<maplibre_jni::JniRendererFrontend> frontend;

    MapWrapperWithFrontend(mbgl::Map* m, maplibre_jni::JniMapObserver* o, maplibre_jni::JniRendererFrontend* f)
        : map(m), observer(o), frontend(f) {}
};

extern "C"
{

    // New entry that accepts a Kotlin-side RendererFrontend (wrapped by JniRendererFrontend on native side)
    JNIEXPORT jlong JNICALL Java_org_maplibre_kmp_native_internal_Native_nativeNewWithFrontend(JNIEnv *env, jclass, jlong frontendPtr, jobject mapObserverObj, jobject mapOptionsObj, jobject resourceOptionsObj, jobject clientOptionsObj)
    {
        try {
            auto* frontend = fromJavaPointer<maplibre_jni::JniRendererFrontend>(frontendPtr);
            auto* observer = new maplibre_jni::JniMapObserver(env, mapObserverObj);
            mbgl::MapOptions mapOptions = maplibre_jni::MapOptionsConversions::extract(env, mapOptionsObj);
            mbgl::ResourceOptions resourceOptions = maplibre_jni::ResourceOptionsConversions::extract(env, resourceOptionsObj);
            mbgl::ClientOptions clientOptions = maplibre_jni::ClientOptionsConversions::extract(env, clientOptionsObj);
            auto* map = new mbgl::Map(*frontend, *observer, mapOptions, resourceOptions, clientOptions);
            auto* wrapper = new MapWrapperWithFrontend(map, observer, nullptr);
            return toJavaPointer(wrapper);
        } catch (const std::exception& e) {
            throwJavaException(env, "java/lang/RuntimeException", e.what());
            return 0;
        }
    }

    // Deprecated path: constructor via Canvas is no longer supported.
    JNIEXPORT jlong JNICALL Java_org_maplibre_kmp_native_MaplibreMap_nativeNew(JNIEnv *env, jclass, jobject, jint, jint, jfloat, jobject, jobject, jobject, jobject)
    {
        throwJavaException(env, "java/lang/UnsupportedOperationException", "Use MaplibreMap(frontend, ...) constructor");
        return 0;
    }

JNIEXPORT void JNICALL Java_org_maplibre_kmp_native_MaplibreMap_nativeDestroy(JNIEnv *env, jclass, jlong ptr)
{
    delete fromJavaPointer<MapWrapperWithFrontend>(ptr);
}

JNIEXPORT void JNICALL Java_org_maplibre_kmp_native_internal_Native_nativeDestroyWithFrontend(JNIEnv* env, jclass, jlong ptr) {
    delete fromJavaPointer<MapWrapperWithFrontend>(ptr);
}


    JNIEXPORT void JNICALL Java_org_maplibre_kmp_native_MaplibreMap_nativeTriggerRepaint(JNIEnv *env, jclass, jlong ptr)
    {
        auto *wrapper = fromJavaPointer<MapWrapperWithFrontend>(ptr);
        wrapper->map->triggerRepaint();
    }

    JNIEXPORT void JNICALL Java_org_maplibre_kmp_native_MaplibreMap_nativeLoadStyleURL(JNIEnv *env, jclass, jlong ptr, jstring jUrl)
    {
        auto *wrapper = fromJavaPointer<MapWrapperWithFrontend>(ptr);
        const char *url = env->GetStringUTFChars(jUrl, nullptr);
        wrapper->map->getStyle().loadURL(std::string(url));
        env->ReleaseStringUTFChars(jUrl, url);
    }

    JNIEXPORT void JNICALL Java_org_maplibre_kmp_native_MaplibreMap_nativeLoadStyleJSON(JNIEnv *env, jclass, jlong ptr, jstring jJson)
    {
        auto *wrapper = fromJavaPointer<MapWrapperWithFrontend>(ptr);
        const char *json = env->GetStringUTFChars(jJson, nullptr);
        wrapper->map->getStyle().loadJSON(std::string(json));
        env->ReleaseStringUTFChars(jJson, json);
    }

    JNIEXPORT void JNICALL Java_org_maplibre_kmp_native_MaplibreMap_nativeJumpTo(JNIEnv *env, jclass, jlong ptr, jobject cameraOptions)
    {
        auto *wrapper = fromJavaPointer<MapWrapperWithFrontend>(ptr);
        mbgl::CameraOptions options = maplibre_jni::CameraOptionsConversions::extract(env, cameraOptions);
        wrapper->map->jumpTo(options);
    }

    JNIEXPORT void JNICALL Java_org_maplibre_kmp_native_MaplibreMap_nativeEaseTo(JNIEnv *env, jclass, jlong ptr, jobject cameraOptions, jint duration)
    {
        auto *wrapper = fromJavaPointer<MapWrapperWithFrontend>(ptr);
        mbgl::CameraOptions options = maplibre_jni::CameraOptionsConversions::extract(env, cameraOptions);

        mbgl::AnimationOptions animationOptions;
        animationOptions.duration = mbgl::Duration(std::chrono::milliseconds(duration));

        wrapper->map->easeTo(options, animationOptions);
    }

    JNIEXPORT void JNICALL Java_org_maplibre_kmp_native_MaplibreMap_nativeFlyTo(JNIEnv *env, jclass, jlong ptr, jobject cameraOptions, jint duration)
    {
        auto *wrapper = fromJavaPointer<MapWrapperWithFrontend>(ptr);
        mbgl::CameraOptions options = maplibre_jni::CameraOptionsConversions::extract(env, cameraOptions);

        mbgl::AnimationOptions animationOptions;
        animationOptions.duration = mbgl::Duration(std::chrono::milliseconds(duration));

        wrapper->map->flyTo(options, animationOptions);
    }

    JNIEXPORT jobject JNICALL Java_org_maplibre_kmp_native_MaplibreMap_nativeGetCameraOptions(JNIEnv *env, jclass, jlong ptr)
    {
        auto *wrapper = fromJavaPointer<MapWrapperWithFrontend>(ptr);
        auto cameraOptions = wrapper->map->getCameraOptions();
        return maplibre_jni::CameraOptionsConversions::create(env, cameraOptions);
    }

    JNIEXPORT void JNICALL Java_org_maplibre_kmp_native_MaplibreMap_nativeSetSize(JNIEnv *env, jclass, jlong ptr, jobject size)
    {
        try
        {
            auto *wrapper = fromJavaPointer<MapWrapperWithFrontend>(ptr);
            mbgl::Size mbglSize = maplibre_jni::SizeConversions::extract(env, size);
            wrapper->map->setSize(mbglSize);
        }
        catch (const std::exception &e)
        {
            throwJavaException(env, "java/lang/RuntimeException", e.what());
        }
    }


    JNIEXPORT void JNICALL Java_org_maplibre_kmp_native_MaplibreMap_nativeMoveBy(JNIEnv *env, jclass, jlong ptr, jobject screenCoordinate)
    {
        try
        {
            auto *wrapper = fromJavaPointer<MapWrapperWithFrontend>(ptr);
            mbgl::ScreenCoordinate coord = maplibre_jni::ScreenCoordinateConversions::extract(env, screenCoordinate);
            wrapper->map->moveBy(coord);
        }
        catch (const std::exception &e)
        {
            throwJavaException(env, "java/lang/RuntimeException", e.what());
        }
    }

    JNIEXPORT void JNICALL Java_org_maplibre_kmp_native_MaplibreMap_nativeScaleBy(JNIEnv *env, jclass, jlong ptr, jdouble scale, jobject anchor)
    {
        try
        {
            auto *wrapper = fromJavaPointer<MapWrapperWithFrontend>(ptr);
            if (anchor != nullptr)
            {
                mbgl::ScreenCoordinate anchorCoord = maplibre_jni::ScreenCoordinateConversions::extract(env, anchor);
                wrapper->map->scaleBy(scale, anchorCoord);
            }
            else
            {
                wrapper->map->scaleBy(scale, std::nullopt);
            }
        }
        catch (const std::exception &e)
        {
            throwJavaException(env, "java/lang/RuntimeException", e.what());
        }
    }

    JNIEXPORT void JNICALL Java_org_maplibre_kmp_native_MaplibreMap_nativeRotateBy(JNIEnv *env, jclass, jlong ptr, jobject first, jobject second)
    {
        try
        {
            auto *wrapper = fromJavaPointer<MapWrapperWithFrontend>(ptr);
            mbgl::ScreenCoordinate firstCoord = maplibre_jni::ScreenCoordinateConversions::extract(env, first);
            mbgl::ScreenCoordinate secondCoord = maplibre_jni::ScreenCoordinateConversions::extract(env, second);
            wrapper->map->rotateBy(firstCoord, secondCoord);
        }
        catch (const std::exception &e)
        {
            throwJavaException(env, "java/lang/RuntimeException", e.what());
        }
    }

    JNIEXPORT void JNICALL Java_org_maplibre_kmp_native_MaplibreMap_nativePitchBy(JNIEnv *env, jclass, jlong ptr, jdouble pitch)
    {
        try
        {
            auto *wrapper = fromJavaPointer<MapWrapperWithFrontend>(ptr);
            wrapper->map->pitchBy(pitch);
        }
        catch (const std::exception &e)
        {
            throwJavaException(env, "java/lang/RuntimeException", e.what());
        }
    }

    JNIEXPORT void JNICALL Java_org_maplibre_kmp_native_MaplibreMap_nativeSetGestureInProgress(JNIEnv *env, jclass, jlong ptr, jboolean inProgress)
    {
        try
        {
            auto *wrapper = fromJavaPointer<MapWrapperWithFrontend>(ptr);
            wrapper->map->setGestureInProgress(inProgress == JNI_TRUE);
        }
        catch (const std::exception &e)
        {
            throwJavaException(env, "java/lang/RuntimeException", e.what());
        }
    }

    JNIEXPORT jobject JNICALL Java_org_maplibre_kmp_native_MaplibreMap_nativePixelForLatLng(JNIEnv *env, jclass, jlong ptr, jobject latLng)
    {
        try
        {
            auto *wrapper = fromJavaPointer<MapWrapperWithFrontend>(ptr);
            mbgl::LatLng coord = maplibre_jni::LatLngConversions::extract(env, latLng);
            mbgl::ScreenCoordinate screenCoord = wrapper->map->pixelForLatLng(coord);
            return maplibre_jni::ScreenCoordinateConversions::create(env, screenCoord);
        }
        catch (const std::exception &e)
        {
            throwJavaException(env, "java/lang/RuntimeException", e.what());
            return nullptr;
        }
    }

    JNIEXPORT jobject JNICALL Java_org_maplibre_kmp_native_MaplibreMap_nativeLatLngForPixel(JNIEnv *env, jclass, jlong ptr, jobject screenCoordinate)
    {
        try
        {
            auto *wrapper = fromJavaPointer<MapWrapperWithFrontend>(ptr);
            mbgl::ScreenCoordinate coord = maplibre_jni::ScreenCoordinateConversions::extract(env, screenCoordinate);
            mbgl::LatLng latLng = wrapper->map->latLngForPixel(coord);
            return maplibre_jni::LatLngConversions::create(env, latLng);
        }
        catch (const std::exception &e)
        {
            throwJavaException(env, "java/lang/RuntimeException", e.what());
            return nullptr;
        }
    }

    JNIEXPORT void JNICALL Java_org_maplibre_kmp_native_MaplibreMap_nativeSetDebug(JNIEnv *env, jclass, jlong ptr, jint debugOptions)
    {
        try
        {
            auto *wrapper = fromJavaPointer<MapWrapperWithFrontend>(ptr);
            wrapper->map->setDebug(static_cast<mbgl::MapDebugOptions>(debugOptions));
        }
        catch (const std::exception &e)
        {
            throwJavaException(env, "java/lang/RuntimeException", e.what());
        }
    }

    JNIEXPORT jint JNICALL Java_org_maplibre_kmp_native_MaplibreMap_nativeGetDebug(JNIEnv *env, jclass, jlong ptr)
    {
        try
        {
            auto *wrapper = fromJavaPointer<MapWrapperWithFrontend>(ptr);
            return static_cast<jint>(wrapper->map->getDebug());
        }
        catch (const std::exception &e)
        {
            throwJavaException(env, "java/lang/RuntimeException", e.what());
            return 0;
        }
    }

    JNIEXPORT void JNICALL Java_org_maplibre_kmp_native_MaplibreMap_nativeEnableRenderingStatsView(JNIEnv *env, jclass, jlong ptr, jboolean enabled)
    {
        try
        {
            auto *wrapper = fromJavaPointer<MapWrapperWithFrontend>(ptr);
            wrapper->map->enableRenderingStatsView(enabled == JNI_TRUE);
        }
        catch (const std::exception &e)
        {
            throwJavaException(env, "java/lang/RuntimeException", e.what());
        }
    }

    JNIEXPORT jboolean JNICALL Java_org_maplibre_kmp_native_MaplibreMap_nativeIsRenderingStatsViewEnabled(JNIEnv *env, jclass, jlong ptr)
    {
        try
        {
            auto *wrapper = fromJavaPointer<MapWrapperWithFrontend>(ptr);
            return wrapper->map->isRenderingStatsViewEnabled() ? JNI_TRUE : JNI_FALSE;
        }
        catch (const std::exception &e)
        {
            throwJavaException(env, "java/lang/RuntimeException", e.what());
            return JNI_FALSE;
        }
    }
}
