#ifdef USE_WGL_BACKEND

#include "awt_wgl_backend.hpp"
#include <mbgl/gl/context.hpp>
#include <mbgl/gl/renderable_resource.hpp>
#include <mbgl/util/logging.hpp>

#include <jawt.h>
#include <jawt_md.h>

#include "gl_functions_wgl.h"

// Forward declaration
namespace maplibre_jni
{
    class WGLBackend;
}

// WGLRenderableResource in global namespace to match GLFW pattern
class WGLRenderableResource final : public mbgl::gl::RenderableResource
{
public:
    explicit WGLRenderableResource(maplibre_jni::WGLBackend &backend_)
        : backend(backend_) {}

    void bind() override;
    void swap() override;

private:
    maplibre_jni::WGLBackend &backend;
};

namespace maplibre_jni
{

    WGLBackend::WGLBackend(JNIEnv *env, jobject canvas, int width, int height)
        : mbgl::gl::RendererBackend(mbgl::gfx::ContextMode::Unique),
          mbgl::gfx::Renderable(
              mbgl::Size{static_cast<uint32_t>(width), static_cast<uint32_t>(height)},
              std::make_unique<WGLRenderableResource>(*this)),
          size({static_cast<uint32_t>(width), static_cast<uint32_t>(height)})
    {
        env->GetJavaVM(&javaVM);
        canvasRef = env->NewGlobalRef(canvas);
        setupWGLContext(env, canvas);
    }

    WGLBackend::~WGLBackend()
    {
        destroyWGLContext();

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

    mbgl::gfx::Renderable &WGLBackend::getDefaultRenderable()
    {
        return *this;
    }

    void WGLBackend::setSize(mbgl::Size newSize)
    {
        // Update both our local size and the Renderable's size
        size = newSize;
        this->mbgl::gfx::Renderable::size = newSize;
    }

    void WGLBackend::activate()
    {
        if (hdc && hglrc)
        {
            if (!wglMakeCurrent(hdc, hglrc))
            {
                mbgl::Log::Error(mbgl::Event::OpenGL, "Failed to make WGL context current");
            }
        }
    }

    void WGLBackend::deactivate()
    {
        if (hdc)
        {
            wglMakeCurrent(nullptr, nullptr);
        }
    }

    mbgl::gl::ProcAddress WGLBackend::getExtensionFunctionPointer(const char *name)
    {
        return reinterpret_cast<mbgl::gl::ProcAddress>(wgl_GetProcAddress(name));
    }

    void WGLBackend::updateAssumedState()
    {
        // Reset GL state assumptions
        assumeFramebufferBinding(0);
        setViewport(0, 0, size);
    }

    void WGLBackend::swapBuffers()
    {
        if (hdc)
        {
            SwapBuffers(hdc);
        }
    }

    JNIEnv *WGLBackend::getEnv()
    {
        JNIEnv *env = nullptr;
        if (javaVM)
        {
            javaVM->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6);
        }
        return env;
    }

    void WGLBackend::setupWGLContext(JNIEnv *env, jobject canvas)
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

        // Store native handles for WGL
        hwnd = win32Info->hwnd;
        // Note: We'll get our own DC from the HWND rather than using JAWT's HDC
        // as JAWT's HDC might not be suitable for OpenGL

        mbgl::Log::Info(mbgl::Event::OpenGL, "JAWT Win32 surface extracted successfully");

        // We must unlock the surface immediately after getting the handles
        // Otherwise AWT event processing will be blocked
        ds->FreeDrawingSurfaceInfo(dsi);
        ds->Unlock(ds);
        awt.FreeDrawingSurface(ds);

        // Initialize WGL
        // Get our own DC from the HWND
        hdc = GetDC(hwnd);
        if (!hdc)
        {
            mbgl::Log::Error(mbgl::Event::OpenGL, "Failed to get DC from HWND");
            return;
        }

        PIXELFORMATDESCRIPTOR pfd;
        ZeroMemory(&pfd, sizeof(pfd));
        pfd.nSize = sizeof(pfd);
        pfd.nVersion = 1;
        pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cColorBits = 24;
        pfd.cAlphaBits = 8;
        pfd.cDepthBits = 24;
        pfd.cStencilBits = 8;

        // Check if pixel format is already set by AWT
        int pixelFormat = ChoosePixelFormat(hdc, &pfd);
        if (pixelFormat == 0)
        {
            DWORD error = GetLastError();
            mbgl::Log::Error(mbgl::Event::OpenGL,
                             std::string("Failed to choose pixel format, error: ") + std::to_string(error));
            return;
        }

        if (!SetPixelFormat(hdc, pixelFormat, &pfd))
        {
            mbgl::Log::Error(mbgl::Event::OpenGL, "Failed to set pixel format");
            return;
        }

        // Create temporary context to load WGL extensions
        HGLRC tempContext = wglCreateContext(hdc);
        if (!tempContext)
        {
            mbgl::Log::Error(mbgl::Event::OpenGL, "Failed to create temporary WGL context");
            return;
        }

        if (!wglMakeCurrent(hdc, tempContext))
        {
            mbgl::Log::Error(mbgl::Event::OpenGL, "Failed to make temporary context current");
            wglDeleteContext(tempContext);
            return;
        }

        // Create modern OpenGL context using WGL_ARB_create_context
        const int contextAttribs[] = {
            WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
            WGL_CONTEXT_MINOR_VERSION_ARB, 0,
            WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
            0};

        hglrc = mbgl::platform::wglCreateContextAttribsARB(hdc, nullptr, contextAttribs);
        if (!hglrc)
        {
            mbgl::Log::Error(mbgl::Event::OpenGL, "Failed to create OpenGL 3.0 context");
            wglMakeCurrent(nullptr, nullptr);
            wglDeleteContext(tempContext);
            return;
        }

        // Switch to the new context and delete the temporary one
        wglMakeCurrent(nullptr, nullptr);
        wglDeleteContext(tempContext);

        if (!wglMakeCurrent(hdc, hglrc))
        {
            mbgl::Log::Error(mbgl::Event::OpenGL, "Failed to make WGL context current");
            return;
        }

        mbgl::Log::Info(mbgl::Event::OpenGL, "WGL context created successfully");
    }

    void WGLBackend::destroyWGLContext()
    {
        if (hglrc)
        {
            wglMakeCurrent(nullptr, nullptr);
            wglDeleteContext(hglrc);
            hglrc = nullptr;
        }

        if (hdc && hwnd)
        {
            ReleaseDC(hwnd, hdc);
            hdc = nullptr;
        }
    }

} // namespace maplibre_jni

void WGLRenderableResource::bind()
{
    backend.setFramebufferBinding(0);
    backend.setViewport(0, 0, backend.getSize());
}

void WGLRenderableResource::swap()
{
    backend.swapBuffers();
}

#endif // USE_WGL_BACKEND
