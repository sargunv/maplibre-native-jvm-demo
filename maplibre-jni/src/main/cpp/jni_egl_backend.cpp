#include "jni_egl_backend.hpp"
#include <mbgl/gl/renderable_resource.hpp>
#include <mbgl/util/instrumentation.hpp>
#include <mbgl/util/logging.hpp>
#include <stdexcept>
#include <sstream>

// OpenGL ES headers
#include <GLES3/gl3.h>

// JAWT includes for native window access
#include <jawt.h>
#include <jawt_md.h>

// Define JAWT_VERSION_9 if not available
#ifndef JAWT_VERSION_9
#define JAWT_VERSION_9 0x00090000
#endif

// Platform-specific includes
#ifdef __APPLE__
    #include <dlfcn.h>
    // For ANGLE Metal backend selection
    #ifndef EGL_PLATFORM_ANGLE_ANGLE
        #define EGL_PLATFORM_ANGLE_ANGLE 0x3202
    #endif
    #ifndef EGL_PLATFORM_ANGLE_TYPE_ANGLE
        #define EGL_PLATFORM_ANGLE_TYPE_ANGLE 0x3203
    #endif
    #ifndef EGL_PLATFORM_ANGLE_TYPE_METAL_ANGLE
        #define EGL_PLATFORM_ANGLE_TYPE_METAL_ANGLE 0x3489
    #endif
#elif _WIN32
    #include <windows.h>
    #ifndef EGL_PLATFORM_ANGLE_ANGLE
        #define EGL_PLATFORM_ANGLE_ANGLE 0x3202
    #endif
    #ifndef EGL_PLATFORM_ANGLE_TYPE_ANGLE
        #define EGL_PLATFORM_ANGLE_TYPE_ANGLE 0x3203
    #endif
    #ifndef EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE
        #define EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE 0x3208
    #endif
#endif

namespace maplibre_jni {

// Custom renderable resource for EGL backend
class EGLRenderableResource final : public mbgl::gl::RenderableResource {
public:
    explicit EGLRenderableResource(EGLRendererBackend& backend_)
        : backend(backend_) {}

    void bind() override {
        MLN_TRACE_FUNC();
        
        // Bind default framebuffer
        backend.setFramebufferBinding(0);
        
        // Get the current size and set viewport
        auto size = backend.getSize();
        backend.setViewport(0, 0, size);
    }

    void swap() override {
        MLN_TRACE_FUNC();
        backend.swap();
    }

private:
    EGLRendererBackend& backend;
};

EGLRendererBackend::EGLRendererBackend(JNIEnv* env, jobject canvas, int width, int height)
    : mbgl::gl::RendererBackend(mbgl::gfx::ContextMode::Unique),
      mbgl::gfx::Renderable(mbgl::Size{ static_cast<uint32_t>(width), static_cast<uint32_t>(height) },
                           std::make_unique<EGLRenderableResource>(*this)) {
    
    // Get JavaVM
    if (env->GetJavaVM(&jvm) != JNI_OK) {
        throw std::runtime_error("Failed to get JavaVM");
    }
    
    // Create global reference to canvas
    canvasRef = env->NewGlobalRef(canvas);
    if (!canvasRef) {
        throw std::runtime_error("Failed to create global reference to canvas");
    }
    
    // Get native window handle
    nativeWindow = getNativeWindowHandle(env, canvas);
    if (!nativeWindow) {
        env->DeleteGlobalRef(canvasRef);
        throw std::runtime_error("Failed to get native window handle");
    }
    
    // Initialize EGL
    initializeEGL();
}

EGLRendererBackend::~EGLRendererBackend() {
    // Clean up EGL resources
    if (display != EGL_NO_DISPLAY) {
        eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        
        if (surface != EGL_NO_SURFACE) {
            eglDestroySurface(display, surface);
        }
        
        if (context != EGL_NO_CONTEXT) {
            eglDestroyContext(display, context);
        }
        
        eglTerminate(display);
    }
    
    // Release native window handle
    releaseNativeWindowHandle();
    
    // Clean up JNI references
    JNIEnv* env = getEnv();
    if (env && canvasRef) {
        env->DeleteGlobalRef(canvasRef);
    }
}

void* EGLRendererBackend::getNativeWindowHandle(JNIEnv* env, jobject canvas) {
#ifdef __APPLE__
    // On macOS, use the Objective-C++ implementation
    return getNativeWindowHandleMacOS(env, canvas);
#else
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
    
    void* handle = nullptr;
    
#ifdef _WIN32
    // Windows: Get HWND
    JAWT_Win32DrawingSurfaceInfo* dsi_win = (JAWT_Win32DrawingSurfaceInfo*)dsi->platformInfo;
    handle = (void*)dsi_win->hwnd;
#else
    // Linux: Get X11 Window
    JAWT_X11DrawingSurfaceInfo* dsi_x11 = (JAWT_X11DrawingSurfaceInfo*)dsi->platformInfo;
    handle = (void*)dsi_x11->drawable;
#endif
    
    // Keep the surface locked - we'll release it in destructor
    // Note: Some platforms may require keeping it locked for the surface to remain valid
    
    // Clean up (but keep the lock)
    ds->FreeDrawingSurfaceInfo(dsi);
    // Don't unlock or free the surface yet - keep it for the lifetime of the backend
    
    return handle;
#endif // !__APPLE__
}

void EGLRendererBackend::releaseNativeWindowHandle() {
#ifdef __APPLE__
    releaseNativeWindowHandleMacOS();
#else
    // In a real implementation, we'd need to properly unlock and free the JAWT surface
    // This is simplified for now
#endif
}

EGLDisplay EGLRendererBackend::getPlatformDisplay() {
    EGLDisplay platformDisplay = EGL_NO_DISPLAY;
    
#ifdef __APPLE__
    // macOS: Use ANGLE with Metal backend
    EGLint displayAttribs[] = {
        EGL_PLATFORM_ANGLE_TYPE_ANGLE, EGL_PLATFORM_ANGLE_TYPE_METAL_ANGLE,
        EGL_NONE
    };
    
    // Try to get eglGetPlatformDisplayEXT function
    PFNEGLGETPLATFORMDISPLAYEXTPROC eglGetPlatformDisplayEXT = 
        (PFNEGLGETPLATFORMDISPLAYEXTPROC)eglGetProcAddress("eglGetPlatformDisplayEXT");
    
    if (eglGetPlatformDisplayEXT) {
        platformDisplay = eglGetPlatformDisplayEXT(EGL_PLATFORM_ANGLE_ANGLE, (void*)EGL_DEFAULT_DISPLAY, displayAttribs);
    }
#elif _WIN32
    // Windows: Use ANGLE with D3D11 backend
    EGLint displayAttribs[] = {
        EGL_PLATFORM_ANGLE_TYPE_ANGLE, EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE,
        EGL_NONE
    };
    
    PFNEGLGETPLATFORMDISPLAYEXTPROC eglGetPlatformDisplayEXT = 
        (PFNEGLGETPLATFORMDISPLAYEXTPROC)eglGetProcAddress("eglGetPlatformDisplayEXT");
    
    if (eglGetPlatformDisplayEXT) {
        platformDisplay = eglGetPlatformDisplayEXT(EGL_PLATFORM_ANGLE_ANGLE, (void*)EGL_DEFAULT_DISPLAY, displayAttribs);
    }
#else
    // Linux: Use native EGL (Mesa) or ANGLE with OpenGL backend
    platformDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
#endif
    
    if (platformDisplay == EGL_NO_DISPLAY) {
        // Fallback to default display
        platformDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    }
    
    return platformDisplay;
}

void EGLRendererBackend::initializeEGL() {
    // Get platform-specific display
    display = getPlatformDisplay();
    if (display == EGL_NO_DISPLAY) {
        throw std::runtime_error("Failed to get EGL display");
    }
    
    // Initialize EGL
    EGLint major, minor;
    if (!eglInitialize(display, &major, &minor)) {
        throw std::runtime_error("Failed to initialize EGL");
    }
    
    mbgl::Log::Info(mbgl::Event::OpenGL, "EGL version: " + std::to_string(major) + "." + std::to_string(minor));
    
    // Bind OpenGL ES API
    if (!eglBindAPI(EGL_OPENGL_ES_API)) {
        throw std::runtime_error("Failed to bind OpenGL ES API");
    }
    
    // Choose config
    EGLint configAttribs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 16,
        EGL_STENCIL_SIZE, 8,
        EGL_NONE
    };
    
    EGLint numConfigs;
    if (!eglChooseConfig(display, configAttribs, &config, 1, &numConfigs) || numConfigs == 0) {
        throw std::runtime_error("Failed to choose EGL config");
    }
    
    // Create context
    EGLint contextAttribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 3,  // Request OpenGL ES 3.0
        EGL_NONE
    };
    
    context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs);
    if (context == EGL_NO_CONTEXT) {
        // Try ES 2.0 as fallback
        contextAttribs[1] = 2;
        context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs);
        if (context == EGL_NO_CONTEXT) {
            throw std::runtime_error("Failed to create EGL context");
        }
    }
    
    // Create window surface
    surface = eglCreateWindowSurface(display, config, (EGLNativeWindowType)nativeWindow, nullptr);
    if (surface == EGL_NO_SURFACE) {
        EGLint error = eglGetError();
        std::ostringstream msg;
        msg << "Failed to create EGL window surface. Error: 0x" << std::hex << error;
        throw std::runtime_error(msg.str());
    }
    
    // Make context current
    if (!eglMakeCurrent(display, surface, surface, context)) {
        throw std::runtime_error("Failed to make EGL context current");
    }
    
    // Log OpenGL ES info
    const char* version = (const char*)glGetString(GL_VERSION);
    const char* renderer = (const char*)glGetString(GL_RENDERER);
    if (version && renderer) {
        mbgl::Log::Info(mbgl::Event::OpenGL, 
            std::string("OpenGL ES: ") + version + " (" + renderer + ")");
    }
}

JNIEnv* EGLRendererBackend::getEnv() {
    JNIEnv* env = nullptr;
    jint result = jvm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6);
    
    if (result == JNI_EDETACHED) {
        // Attach current thread
        if (jvm->AttachCurrentThread(reinterpret_cast<void**>(&env), nullptr) != JNI_OK) {
            return nullptr;
        }
    }
    
    return env;
}

void EGLRendererBackend::activate() {
    MLN_TRACE_FUNC();
    
    if (!eglMakeCurrent(display, surface, surface, context)) {
        throw std::runtime_error("Failed to make EGL context current");
    }
}

void EGLRendererBackend::deactivate() {
    MLN_TRACE_FUNC();
    
    eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
}

void EGLRendererBackend::updateAssumedState() {
    MLN_TRACE_FUNC();
    
    // Update assumed OpenGL state
    assumeFramebufferBinding(0);
    setViewport(0, 0, size);
}

mbgl::gl::ProcAddress EGLRendererBackend::getExtensionFunctionPointer(const char* name) {
    MLN_TRACE_FUNC();
    
    return reinterpret_cast<mbgl::gl::ProcAddress>(eglGetProcAddress(name));
}

void EGLRendererBackend::updateSize(int width, int height) {
    size = mbgl::Size{ static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
}

void EGLRendererBackend::swap() {
    eglSwapBuffers(display, surface);
}

} // namespace maplibre_jni