#ifndef __APPLE__

#include "awt_opengl_backend.hpp"
#include <mbgl/gl/context.hpp>
#include <mbgl/gl/renderable_resource.hpp>
#include <mbgl/util/logging.hpp>

#include <jawt.h>
#include <jawt_md.h>

#ifdef __linux__
#include <X11/Xlib.h>
#endif

#ifdef _WIN32
#include <windows.h>
#endif

#include <EGL/eglext.h>

// Forward declaration
namespace maplibre_jni {
    class OpenGLBackend;
}

// OpenGLRenderableResource in global namespace to match GLFW pattern
class OpenGLRenderableResource final : public mbgl::gl::RenderableResource
{
public:
    explicit OpenGLRenderableResource(maplibre_jni::OpenGLBackend& backend_)
        : backend(backend_) {}

    void bind() override;
    void swap() override;

private:
    maplibre_jni::OpenGLBackend& backend;
};

namespace maplibre_jni
{

    OpenGLBackend::OpenGLBackend(JNIEnv *env, jobject canvas, int width, int height)
        : mbgl::gl::RendererBackend(mbgl::gfx::ContextMode::Unique),
          mbgl::gfx::Renderable(
              mbgl::Size{static_cast<uint32_t>(width), static_cast<uint32_t>(height)},
              std::make_unique<OpenGLRenderableResource>(*this)),
          size({static_cast<uint32_t>(width), static_cast<uint32_t>(height)})
    {
        env->GetJavaVM(&javaVM);
        canvasRef = env->NewGlobalRef(canvas);
        setupOpenGLContext(env, canvas);
    }

    OpenGLBackend::~OpenGLBackend()
    {
        destroyOpenGLContext();

        if (canvasRef)
        {
            JNIEnv *env = getEnv();
            if (env)
            {
                env->DeleteGlobalRef(canvasRef);
            }
            canvasRef = nullptr;
        }
    }

    mbgl::gfx::Renderable &OpenGLBackend::getDefaultRenderable()
    {
        return *this;
    }

    void OpenGLBackend::setSize(mbgl::Size newSize)
    {
        // Update both our local size and the Renderable's size
        size = newSize;
        this->mbgl::gfx::Renderable::size = newSize;
    }

    void OpenGLBackend::activate()
    {
        if (eglDisplay != EGL_NO_DISPLAY && eglContext != EGL_NO_CONTEXT && eglSurface != EGL_NO_SURFACE)
        {
            if (!eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext))
            {
                mbgl::Log::Error(mbgl::Event::OpenGL, "Failed to make EGL context current");
            }
        }
    }

    void OpenGLBackend::deactivate()
    {
        if (eglDisplay != EGL_NO_DISPLAY)
        {
            eglMakeCurrent(eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        }
    }

    mbgl::gl::ProcAddress OpenGLBackend::getExtensionFunctionPointer(const char *name)
    {
        return eglGetProcAddress(name);
    }

    void OpenGLBackend::updateAssumedState()
    {
        // Reset GL state assumptions
        assumeFramebufferBinding(0);
        setViewport(0, 0, size);
    }
    
    void OpenGLBackend::swapBuffers()
    {
        if (eglDisplay != EGL_NO_DISPLAY && eglSurface != EGL_NO_SURFACE)
        {
            eglSwapBuffers(eglDisplay, eglSurface);
        }
    }

    JNIEnv *OpenGLBackend::getEnv()
    {
        JNIEnv *env = nullptr;
        if (javaVM)
        {
            javaVM->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6);
        }
        return env;
    }

    void OpenGLBackend::setupOpenGLContext(JNIEnv *env, jobject canvas)
    {
        // Get JAWT
        JAWT awt;
        awt.version = JAWT_VERSION_9;

        jboolean result = JAWT_GetAWT(env, &awt);
        if (result == JNI_FALSE)
        {
            mbgl::Log::Error(mbgl::Event::OpenGL, "JAWT_GetAWT failed");
            return;
        }

        // Get the drawing surface
        JAWT_DrawingSurface *ds = awt.GetDrawingSurface(env, canvas);
        if (!ds)
        {
            mbgl::Log::Error(mbgl::Event::OpenGL, "GetDrawingSurface returned null");
            return;
        }

        // Lock the drawing surface
        jint lock = ds->Lock(ds);
        if ((lock & JAWT_LOCK_ERROR) != 0)
        {
            mbgl::Log::Error(mbgl::Event::OpenGL, "Error locking drawing surface");
            awt.FreeDrawingSurface(ds);
            return;
        }

        // Get the drawing surface info
        JAWT_DrawingSurfaceInfo *dsi = ds->GetDrawingSurfaceInfo(ds);
        if (!dsi)
        {
            mbgl::Log::Error(mbgl::Event::OpenGL, "GetDrawingSurfaceInfo returned null");
            ds->Unlock(ds);
            awt.FreeDrawingSurface(ds);
            return;
        }

#ifdef __linux__
        // Get the X11 drawing surface info
        JAWT_X11DrawingSurfaceInfo *x11Info = (JAWT_X11DrawingSurfaceInfo *)dsi->platformInfo;
        if (!x11Info)
        {
            mbgl::Log::Error(mbgl::Event::OpenGL, "Platform info is null");
            ds->FreeDrawingSurfaceInfo(dsi);
            ds->Unlock(ds);
            awt.FreeDrawingSurface(ds);
            return;
        }

        // Store native handles for EGL
        nativeDisplay = x11Info->display;
        // X11 drawable is already the correct type (Window/XID)
        nativeWindow = reinterpret_cast<void*>(x11Info->drawable);

        mbgl::Log::Info(mbgl::Event::OpenGL, "JAWT X11 surface extracted successfully");
#elif _WIN32
        // Get the Windows drawing surface info
        JAWT_Win32DrawingSurfaceInfo *win32Info = (JAWT_Win32DrawingSurfaceInfo *)dsi->platformInfo;
        if (!win32Info)
        {
            mbgl::Log::Error(mbgl::Event::OpenGL, "Platform info is null");
            ds->FreeDrawingSurfaceInfo(dsi);
            ds->Unlock(ds);
            awt.FreeDrawingSurface(ds);
            return;
        }

        // Store native handles for EGL
        nativeDisplay = EGL_DEFAULT_DISPLAY;
        // HWND is already a pointer type
        nativeWindow = win32Info->hwnd;

        mbgl::Log::Info(mbgl::Event::OpenGL, "JAWT Win32 surface extracted successfully");
#endif

        // We must unlock the surface immediately after getting the handles
        // Otherwise AWT event processing will be blocked
        ds->FreeDrawingSurfaceInfo(dsi);
        ds->Unlock(ds);
        awt.FreeDrawingSurface(ds);

        // Initialize EGL
        eglDisplay = eglGetDisplay(static_cast<EGLNativeDisplayType>(nativeDisplay));
        if (eglDisplay == EGL_NO_DISPLAY)
        {
            mbgl::Log::Error(mbgl::Event::OpenGL, "Failed to get EGL display");
            return;
        }

        EGLint major, minor;
        if (!eglInitialize(eglDisplay, &major, &minor))
        {
            mbgl::Log::Error(mbgl::Event::OpenGL, "Failed to initialize EGL");
            return;
        }

        mbgl::Log::Info(mbgl::Event::OpenGL,
                        std::string("EGL initialized: ") + std::to_string(major) + "." + std::to_string(minor));

        // Choose EGL config
        const EGLint configAttribs[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_ALPHA_SIZE, 8,
            EGL_DEPTH_SIZE, 24,
            EGL_STENCIL_SIZE, 8,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_NONE};

        EGLint numConfigs;
        if (!eglChooseConfig(eglDisplay, configAttribs, &eglConfig, 1, &numConfigs) || numConfigs == 0)
        {
            mbgl::Log::Error(mbgl::Event::OpenGL, "Failed to choose EGL config");
            return;
        }

        // Create EGL surface
#ifdef __linux__
        eglSurface = eglCreateWindowSurface(eglDisplay, eglConfig,
                                            reinterpret_cast<EGLNativeWindowType>(nativeWindow),
                                            nullptr);
#elif _WIN32
        eglSurface = eglCreateWindowSurface(eglDisplay, eglConfig,
                                            static_cast<EGLNativeWindowType>(nativeWindow),
                                            nullptr);
#endif
        if (eglSurface == EGL_NO_SURFACE)
        {
            mbgl::Log::Error(mbgl::Event::OpenGL, "Failed to create EGL surface");
            return;
        }

        // Create EGL context
        const EGLint contextAttribs[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE};

        eglContext = eglCreateContext(eglDisplay, eglConfig, EGL_NO_CONTEXT, contextAttribs);
        if (eglContext == EGL_NO_CONTEXT)
        {
            mbgl::Log::Error(mbgl::Event::OpenGL, "Failed to create EGL context");
            return;
        }

        // Make context current
        if (!eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext))
        {
            mbgl::Log::Error(mbgl::Event::OpenGL, "Failed to make EGL context current");
            return;
        }

        mbgl::Log::Info(mbgl::Event::OpenGL, "OpenGL context created successfully");
    }

    void OpenGLBackend::destroyOpenGLContext()
    {
        if (eglDisplay != EGL_NO_DISPLAY)
        {
            eglMakeCurrent(eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

            if (eglContext != EGL_NO_CONTEXT)
            {
                eglDestroyContext(eglDisplay, eglContext);
                eglContext = EGL_NO_CONTEXT;
            }

            if (eglSurface != EGL_NO_SURFACE)
            {
                eglDestroySurface(eglDisplay, eglSurface);
                eglSurface = EGL_NO_SURFACE;
            }

            eglTerminate(eglDisplay);
            eglDisplay = EGL_NO_DISPLAY;
        }
    }

} // namespace maplibre_jni

// Implementation of OpenGLRenderableResource methods
void OpenGLRenderableResource::bind()
{
    backend.setFramebufferBinding(0);
    backend.setViewport(0, 0, backend.getSize());
}

void OpenGLRenderableResource::swap()
{
    backend.swapBuffers();
}

#endif // !__APPLE__
