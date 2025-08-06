#ifdef USE_EGL_BACKEND

#include "awt_egl_backend.hpp"
#include <mbgl/gl/context.hpp>
#include <mbgl/gl/renderable_resource.hpp>
#include <mbgl/util/logging.hpp>

#include <jawt.h>
#include <jawt_md.h>

#ifdef __linux__
#include <X11/Xlib.h>
#endif

// Forward declaration
namespace maplibre_jni
{
    class EGLBackend;
}

// EGLRenderableResource in global namespace to match GLFW pattern
class EGLRenderableResource final : public mbgl::gl::RenderableResource
{
public:
    explicit EGLRenderableResource(maplibre_jni::EGLBackend &backend_)
        : backend(backend_) {}

    void bind() override;
    void swap() override;

private:
    maplibre_jni::EGLBackend &backend;
};

namespace maplibre_jni
{

    EGLBackend::EGLBackend(JNIEnv *env, jobject canvas, int width, int height)
        : mbgl::gl::RendererBackend(mbgl::gfx::ContextMode::Unique),
          mbgl::gfx::Renderable(
              mbgl::Size{static_cast<uint32_t>(width), static_cast<uint32_t>(height)},
              std::make_unique<EGLRenderableResource>(*this)),
          size({static_cast<uint32_t>(width), static_cast<uint32_t>(height)})
    {
        env->GetJavaVM(&javaVM);
        canvasRef = env->NewGlobalRef(canvas);
        setupEGLContext(env, canvas);
    }

    EGLBackend::~EGLBackend()
    {
        destroyEGLContext();

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

    mbgl::gfx::Renderable &EGLBackend::getDefaultRenderable()
    {
        return *this;
    }

    void EGLBackend::setSize(mbgl::Size newSize)
    {
        // Update both our local size and the Renderable's size
        size = newSize;
        this->mbgl::gfx::Renderable::size = newSize;
    }

    void EGLBackend::activate()
    {
        if (eglDisplay != EGL_NO_DISPLAY && eglContext != EGL_NO_CONTEXT && eglSurface != EGL_NO_SURFACE)
        {
            if (!eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext))
            {
                mbgl::Log::Error(mbgl::Event::OpenGL, "Failed to make EGL context current");
            }
        }
    }

    void EGLBackend::deactivate()
    {
        if (eglDisplay != EGL_NO_DISPLAY)
        {
            eglMakeCurrent(eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        }
    }

    mbgl::gl::ProcAddress EGLBackend::getExtensionFunctionPointer(const char *name)
    {
        return eglGetProcAddress(name);
    }

    void EGLBackend::updateAssumedState()
    {
        // Reset GL state assumptions
        assumeFramebufferBinding(0);
        setViewport(0, 0, size);
    }

    void EGLBackend::swapBuffers()
    {
        if (eglDisplay != EGL_NO_DISPLAY && eglSurface != EGL_NO_SURFACE)
        {
            eglSwapBuffers(eglDisplay, eglSurface);
        }
    }

    JNIEnv *EGLBackend::getEnv()
    {
        JNIEnv *env = nullptr;
        if (javaVM)
        {
            javaVM->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6);
        }
        return env;
    }

    void EGLBackend::setupEGLContext(JNIEnv *env, jobject canvas)
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
        nativeWindow = reinterpret_cast<void *>(x11Info->drawable);

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
        hwnd = win32Info->hwnd;
        nativeWindow = hwnd;
        // On Windows with ANGLE, we use EGL_DEFAULT_DISPLAY
        nativeDisplay = EGL_DEFAULT_DISPLAY;

        mbgl::Log::Info(mbgl::Event::OpenGL, "JAWT Win32 surface extracted successfully");
#endif

        // We must unlock the surface immediately after getting the handles
        // Otherwise AWT event processing will be blocked
        ds->FreeDrawingSurfaceInfo(dsi);
        ds->Unlock(ds);
        awt.FreeDrawingSurface(ds);

        // Initialize EGL
#ifdef _WIN32
        // On Windows with ANGLE, use EGL_DEFAULT_DISPLAY
        eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
#else
        // On Linux, use the X11 display
        eglDisplay = eglGetDisplay(static_cast<EGLNativeDisplayType>(nativeDisplay));
#endif

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

        // Bind to OpenGL ES API
        if (!eglBindAPI(EGL_OPENGL_ES_API))
        {
            mbgl::Log::Error(mbgl::Event::OpenGL, "Failed to bind OpenGL ES API");
            return;
        }

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
        eglSurface = eglCreateWindowSurface(eglDisplay, eglConfig,
                                            static_cast<EGLNativeWindowType>(nativeWindow),
                                            nullptr);
        if (eglSurface == EGL_NO_SURFACE)
        {
            EGLint error = eglGetError();
            mbgl::Log::Error(mbgl::Event::OpenGL,
                             std::string("Failed to create EGL surface, error: 0x") +
                                 std::to_string(error));
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

        if (!eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext))
        {
            mbgl::Log::Error(mbgl::Event::OpenGL, "Failed to make EGL context current");
            return;
        }

        mbgl::Log::Info(mbgl::Event::OpenGL, "EGL context created successfully");
    }

    void EGLBackend::destroyEGLContext()
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

void EGLRenderableResource::bind()
{
    backend.setFramebufferBinding(0);
    backend.setViewport(0, 0, backend.getSize());
}

void EGLRenderableResource::swap()
{
    backend.swapBuffers();
}

#endif USE_EGL_BACKEND
