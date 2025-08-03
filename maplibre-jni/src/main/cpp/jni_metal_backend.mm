#ifdef __APPLE__

#include "jni_jawt_backend.hpp"
#include <mbgl/mtl/renderer_backend.hpp>
#include <mbgl/mtl/renderable_resource.hpp>
#include <mbgl/mtl/mtl_fwd.hpp>
#include <mbgl/mtl/texture2d.hpp>
#include <mbgl/gfx/context.hpp>
#include <mbgl/util/logging.hpp>

#include <Metal/Metal.hpp>
#include <QuartzCore/CAMetalLayer.hpp>

#import <Cocoa/Cocoa.h>
#include <jawt.h>
#include <jawt_md.h>

// Define JAWT_VERSION_9 if not available
#ifndef JAWT_VERSION_9
#define JAWT_VERSION_9 0x00090000
#endif

namespace maplibre_jni {

// Metal backend implementation - mimics GLFW structure exactly
class MetalBackend final : public mbgl::mtl::RendererBackend, 
                          public mbgl::gfx::Renderable,
                          public JAWTRendererBackend {
public:
    MetalBackend(JNIEnv* env, jobject canvas, int width, int height);
    ~MetalBackend() override;
    
    // JAWTRendererBackend implementation
    void updateSize(int width, int height) override;
    void swap() override;
    void* getRendererBackend() override { return static_cast<mbgl::mtl::RendererBackend*>(this); }
    
    // mbgl::gfx::RendererBackend implementation
    mbgl::gfx::Renderable& getDefaultRenderable() override { return *this; }
    
    // mbgl::mtl::RendererBackend implementation - exact copy from GLFW
    void activate() override {}
    void deactivate() override {}
    void updateAssumedState() override {}
    
    void setSize(mbgl::Size size_);
    mbgl::Size getSize() const;
    
private:
    void setupMetalLayer(JNIEnv* env, jobject canvas);
    void releaseNativeWindow();
    
    // JAWT structures
    void* jawtDrawingSurface = nullptr;
    void* jawtDrawingSurfaceInfo = nullptr;
};

} // namespace maplibre_jni

namespace mbgl {

using namespace mtl;

// Metal renderable resource - exact copy from GLFW
class MetalRenderableResource final : public mtl::RenderableResource {
public:
    MetalRenderableResource(maplibre_jni::MetalBackend& backend)
        : rendererBackend(backend),
          commandQueue(NS::TransferPtr(backend.getDevice()->newCommandQueue())),
          swapchain(NS::TransferPtr(CA::MetalLayer::layer())) {
        swapchain->setDevice(backend.getDevice().get());
    }
    
    void setBackendSize(mbgl::Size size_) {
        size = size_;
        swapchain->setDrawableSize({static_cast<CGFloat>(size.width), static_cast<CGFloat>(size.height)});
        buffersInvalid = true;
    }
    
    mbgl::Size getSize() const {
        return size;
    }
    
    void bind() override {
        surface = NS::TransferPtr(swapchain->nextDrawable());
        auto texSize = mbgl::Size{
            static_cast<uint32_t>(swapchain->drawableSize().width),
            static_cast<uint32_t>(swapchain->drawableSize().height)};
        
        commandBuffer = NS::TransferPtr(commandQueue->commandBuffer());
        renderPassDescriptor = NS::TransferPtr(MTL::RenderPassDescriptor::renderPassDescriptor());
        renderPassDescriptor->colorAttachments()->object(0)->setTexture(surface->texture());
        
        if (buffersInvalid || !depthTexture || !stencilTexture) {
            buffersInvalid = false;
            depthTexture = rendererBackend.getContext().createTexture2D();
            depthTexture->setSize(texSize);
            depthTexture->setFormat(gfx::TexturePixelType::Depth, gfx::TextureChannelDataType::Float);
            depthTexture->setSamplerConfiguration(
                {gfx::TextureFilterType::Linear, gfx::TextureWrapType::Clamp, gfx::TextureWrapType::Clamp});
            static_cast<mtl::Texture2D*>(depthTexture.get())->setUsage(
                MTL::TextureUsageShaderRead | MTL::TextureUsageShaderWrite | MTL::TextureUsageRenderTarget);
            
            stencilTexture = rendererBackend.getContext().createTexture2D();
            stencilTexture->setSize(texSize);
            stencilTexture->setFormat(gfx::TexturePixelType::Stencil, gfx::TextureChannelDataType::UnsignedByte);
            stencilTexture->setSamplerConfiguration(
                {gfx::TextureFilterType::Linear, gfx::TextureWrapType::Clamp, gfx::TextureWrapType::Clamp});
            static_cast<mtl::Texture2D*>(stencilTexture.get())
                ->setUsage(MTL::TextureUsageShaderRead | MTL::TextureUsageShaderWrite | MTL::TextureUsageRenderTarget);
        }
        
        if (depthTexture) {
            depthTexture->create();
            if (auto* depthTarget = renderPassDescriptor->depthAttachment()) {
                depthTarget->setTexture(static_cast<mtl::Texture2D*>(depthTexture.get())->getMetalTexture());
            }
        }
        if (stencilTexture) {
            stencilTexture->create();
            if (auto* stencilTarget = renderPassDescriptor->stencilAttachment()) {
                stencilTarget->setTexture(static_cast<mtl::Texture2D*>(stencilTexture.get())->getMetalTexture());
            }
        }
    }
    
    void swap() override {
        commandBuffer->presentDrawable(surface.get());
        commandBuffer->commit();
        commandBuffer.reset();
        renderPassDescriptor.reset();
    }
    
    const mtl::RendererBackend& getBackend() const override {
        return rendererBackend;
    }
    
    const mtl::MTLCommandBufferPtr& getCommandBuffer() const override {
        return commandBuffer;
    }
    
    mtl::MTLBlitPassDescriptorPtr getUploadPassDescriptor() const override {
        return NS::TransferPtr(MTL::BlitPassDescriptor::alloc()->init());
    }
    
    const mtl::MTLRenderPassDescriptorPtr& getRenderPassDescriptor() const override {
        return renderPassDescriptor;
    }
    
    const CAMetalLayerPtr& getSwapchain() const {
        return swapchain;
    }
    
private:
    maplibre_jni::MetalBackend& rendererBackend;
    MTLCommandQueuePtr commandQueue;
    MTLCommandBufferPtr commandBuffer;
    MTLRenderPassDescriptorPtr renderPassDescriptor;
    CAMetalDrawablePtr surface;
    CAMetalLayerPtr swapchain;
    gfx::Texture2DPtr depthTexture;
    gfx::Texture2DPtr stencilTexture;
    mbgl::Size size;
    bool buffersInvalid = true;
};

} // namespace mbgl

namespace maplibre_jni {

// MetalBackend implementation - mimics GLFW constructor
MetalBackend::MetalBackend(JNIEnv* env, jobject canvas, int width_, int height_)
    : mbgl::mtl::RendererBackend(mbgl::gfx::ContextMode::Unique),
      mbgl::gfx::Renderable(mbgl::Size{0, 0}, std::make_unique<mbgl::MetalRenderableResource>(*this)),
      JAWTRendererBackend(env, canvas, width_, height_) {
    setupMetalLayer(env, canvas);
    setSize(mbgl::Size{static_cast<uint32_t>(width_), static_cast<uint32_t>(height_)});
}

MetalBackend::~MetalBackend() {
    releaseNativeWindow();
}

void MetalBackend::updateSize(int width_, int height_) {
    width = width_;
    height = height_;
    setSize(mbgl::Size{static_cast<uint32_t>(width), static_cast<uint32_t>(height)});
}

void MetalBackend::swap() {
    // Not needed - the resource handles swapping
}

void MetalBackend::setSize(mbgl::Size size_) {
    getResource<mbgl::MetalRenderableResource>().setBackendSize(size_);
}

mbgl::Size MetalBackend::getSize() const { 
    return getResource<mbgl::MetalRenderableResource>().getSize(); 
}

void MetalBackend::setupMetalLayer(JNIEnv* env, jobject canvas) {
    // Get JAWT
    JAWT awt;
    awt.version = JAWT_VERSION_9;
    
    jboolean result = JAWT_GetAWT(env, &awt);
    if (result == JNI_FALSE) {
        mbgl::Log::Error(mbgl::Event::OpenGL, "JAWT_GetAWT failed");
        return;
    }
    
    // Get the drawing surface
    JAWT_DrawingSurface* ds = awt.GetDrawingSurface(env, canvas);
    if (!ds) {
        mbgl::Log::Error(mbgl::Event::OpenGL, "GetDrawingSurface returned null");
        return;
    }
    
    // Lock the drawing surface
    jint lock = ds->Lock(ds);
    if ((lock & JAWT_LOCK_ERROR) != 0) {
        mbgl::Log::Error(mbgl::Event::OpenGL, "Failed to lock drawing surface");
        awt.FreeDrawingSurface(ds);
        return;
    }
    
    // Get the drawing surface info
    JAWT_DrawingSurfaceInfo* dsi = ds->GetDrawingSurfaceInfo(ds);
    if (!dsi) {
        mbgl::Log::Error(mbgl::Event::OpenGL, "GetDrawingSurfaceInfo returned null");
        ds->Unlock(ds);
        awt.FreeDrawingSurface(ds);
        return;
    }
    
    // Get the platform-specific info
    id<JAWT_SurfaceLayers> surfaceLayers = (id<JAWT_SurfaceLayers>)dsi->platformInfo;
    if (!surfaceLayers) {
        mbgl::Log::Error(mbgl::Event::OpenGL, "platformInfo is null");
        ds->FreeDrawingSurfaceInfo(dsi);
        ds->Unlock(ds);
        awt.FreeDrawingSurface(ds);
        return;
    }
    
    // This mimics what GLFW does: window.contentView.layer = metalLayer
    // Get the CAMetalLayer from our renderable resource
    CALayer* metalLayer = (__bridge CALayer*)getDefaultRenderable().getResource<mbgl::MetalRenderableResource>().getSwapchain().get();
    surfaceLayers.layer = metalLayer;
    
    // Store references for cleanup
    jawtDrawingSurface = ds;
    jawtDrawingSurfaceInfo = dsi;
    
    mbgl::Log::Info(mbgl::Event::OpenGL, "Metal layer configured successfully");
}

void MetalBackend::releaseNativeWindow() {
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

// Factory function implementation
std::unique_ptr<JAWTRendererBackend> createPlatformBackend(JNIEnv* env, jobject canvas, int width, int height) {
    return std::make_unique<MetalBackend>(env, canvas, width, height);
}

} // namespace maplibre_jni

#endif // __APPLE__