#include "jni_helpers.hpp"
#include "surface_descriptors.hpp"
#include <mbgl/gfx/renderer_backend.hpp>
#include <mbgl/mtl/renderer_backend.hpp>
#include <mbgl/mtl/renderable_resource.hpp>
#include <mbgl/mtl/texture2d.hpp>
#include <mbgl/gfx/context.hpp>
#include <mbgl/util/size.hpp>
#include <mbgl/util/color.hpp>

#if defined(__APPLE__)
#include <Metal/Metal.hpp>
#include <QuartzCore/CAMetalLayer.hpp>
#endif

using namespace mbgl;
using namespace mbgl::mtl;
using namespace maplibre_jni;

namespace {

class MetalRenderableResourceExt final : public mbgl::mtl::RenderableResource {
public:
    MetalRenderableResourceExt(mtl::RendererBackend& backend, CA::MetalLayer* externalLayer)
        : rendererBackend(backend), commandQueue(NS::TransferPtr(backend.getDevice()->newCommandQueue())), swapchain(NS::RetainPtr(externalLayer)) {
        swapchain->setDevice(backend.getDevice().get());
    }

    void setBackendSize(mbgl::Size size_) {
        size = size_;
        swapchain->setDrawableSize({static_cast<CGFloat>(size.width), static_cast<CGFloat>(size.height)});
        buffersInvalid = true;
    }

    mbgl::Size getSize() const { return size; }

    void bind() override {
        surface = NS::TransferPtr(swapchain->nextDrawable());
        auto texSize = mbgl::Size{ static_cast<uint32_t>(swapchain->drawableSize().width), static_cast<uint32_t>(swapchain->drawableSize().height) };
        commandBuffer = NS::TransferPtr(commandQueue->commandBuffer());
        renderPassDescriptor = NS::TransferPtr(MTL::RenderPassDescriptor::renderPassDescriptor());
        renderPassDescriptor->colorAttachments()->object(0)->setTexture(surface->texture());
        if (buffersInvalid || !depthTexture || !stencilTexture) {
            buffersInvalid = false;
            depthTexture = rendererBackend.getContext().createTexture2D();
            depthTexture->setSize(texSize);
            depthTexture->setFormat(mbgl::gfx::TexturePixelType::Depth, mbgl::gfx::TextureChannelDataType::Float);
            depthTexture->setSamplerConfiguration({mbgl::gfx::TextureFilterType::Linear, mbgl::gfx::TextureWrapType::Clamp, mbgl::gfx::TextureWrapType::Clamp});
            static_cast<mbgl::mtl::Texture2D*>(depthTexture.get())->setUsage(MTL::TextureUsageShaderRead | MTL::TextureUsageShaderWrite | MTL::TextureUsageRenderTarget);
            stencilTexture = rendererBackend.getContext().createTexture2D();
            stencilTexture->setSize(texSize);
            stencilTexture->setFormat(mbgl::gfx::TexturePixelType::Stencil, mbgl::gfx::TextureChannelDataType::UnsignedByte);
            stencilTexture->setSamplerConfiguration({mbgl::gfx::TextureFilterType::Linear, mbgl::gfx::TextureWrapType::Clamp, mbgl::gfx::TextureWrapType::Clamp});
            static_cast<mbgl::mtl::Texture2D*>(stencilTexture.get())->setUsage(MTL::TextureUsageShaderRead | MTL::TextureUsageShaderWrite | MTL::TextureUsageRenderTarget);
        }
        renderPassDescriptor->depthAttachment()->setTexture(static_cast<mbgl::mtl::Texture2D*>(depthTexture.get())->getMetalTexture());
        renderPassDescriptor->stencilAttachment()->setTexture(static_cast<mbgl::mtl::Texture2D*>(stencilTexture.get())->getMetalTexture());
        renderPassDescriptor->colorAttachments()->object(0)->setClearColor(MTL::ClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a));
        renderPassDescriptor->colorAttachments()->object(0)->setLoadAction(MTL::LoadActionClear);
        renderPassDescriptor->colorAttachments()->object(0)->setStoreAction(MTL::StoreActionStore);
        renderPassDescriptor->depthAttachment()->setClearDepth(1.0);
        renderPassDescriptor->depthAttachment()->setLoadAction(MTL::LoadActionClear);
        renderPassDescriptor->depthAttachment()->setStoreAction(MTL::StoreActionDontCare);
        renderPassDescriptor->stencilAttachment()->setClearStencil(0);
        renderPassDescriptor->stencilAttachment()->setLoadAction(MTL::LoadActionClear);
        renderPassDescriptor->stencilAttachment()->setStoreAction(MTL::StoreActionDontCare);
    }

    void swap() override {
        if (commandBuffer) { commandBuffer->presentDrawable(surface.get()); commandBuffer->commit(); }
    }

    const mbgl::mtl::RendererBackend& getBackend() const override { return rendererBackend; }
    const mbgl::mtl::MTLCommandBufferPtr& getCommandBuffer() const override { return commandBuffer; }
    mbgl::mtl::MTLBlitPassDescriptorPtr getUploadPassDescriptor() const override { return mbgl::mtl::MTLBlitPassDescriptorPtr(); }
    const mbgl::mtl::MTLRenderPassDescriptorPtr& getRenderPassDescriptor() const override { return renderPassDescriptor; }

    NS::SharedPtr<CA::MetalLayer> swapchain;

private:
    mtl::RendererBackend& rendererBackend;
    mbgl::Color clearColor{0.0, 0.0, 0.0, 1.0};
    mbgl::Size size{0, 0};
    NS::SharedPtr<MTL::CommandQueue> commandQueue;
    NS::SharedPtr<CA::MetalDrawable> surface;
    mbgl::mtl::MTLCommandBufferPtr commandBuffer;
    mbgl::mtl::MTLRenderPassDescriptorPtr renderPassDescriptor;
    mbgl::gfx::Texture2DPtr depthTexture;
    mbgl::gfx::Texture2DPtr stencilTexture;
    bool buffersInvalid = false;
};

class MetalBackend2 : public mtl::RendererBackend, public gfx::Renderable {
public:
    MetalBackend2(CA::MetalLayer* layer, mbgl::gfx::ContextMode mode)
        : mbgl::mtl::RendererBackend(mode), mbgl::gfx::Renderable(mbgl::Size{0,0}, std::make_unique<MetalRenderableResourceExt>(*this, layer)) {}

    void setSize(mbgl::Size size) { getResource<MetalRenderableResourceExt>().setBackendSize(size); }
    mbgl::gfx::Renderable& getDefaultRenderable() override { return *this; }
    void updateAssumedState() override {}
    void activate() override {}
    void deactivate() override {}
};

} // namespace

extern "C" {

JNIEXPORT jlong JNICALL Java_org_maplibre_kmp_native_internal_Native_createMetalBackend(JNIEnv* env, jclass, jlong surfaceDescPtr, jint width, jint height, jfloat /*pixelRatio*/, jint /*contextMode*/) {
    try {
        auto* desc = fromJavaPointer<SurfaceDescriptor>(surfaceDescPtr);
        if (!desc || desc->kind != SurfaceKind::Mac) throw std::runtime_error("Invalid macOS surface descriptor");
        auto* layer = (CA::MetalLayer*)desc->u.mac.caLayer;
        auto* backend = new MetalBackend2(layer, mbgl::gfx::ContextMode::Unique);
        backend->setSize(mbgl::Size{static_cast<uint32_t>(width), static_cast<uint32_t>(height)});
        return toJavaPointer(static_cast<gfx::RendererBackend*>(backend));
    } catch (const std::exception& e) {
        throwJavaException(env, "java/lang/RuntimeException", e.what());
        return 0;
    }
}

JNIEXPORT jlong JNICALL Java_org_maplibre_kmp_native_internal_Native_createDefaultBackend(JNIEnv* env, jclass, jlong surfaceDescPtr, jint width, jint height, jfloat pixelRatio, jint contextMode) {
    // On Metal builds, default backend is Metal
    return Java_org_maplibre_kmp_native_internal_Native_createMetalBackend(env, nullptr, surfaceDescPtr, width, height, pixelRatio, contextMode);
}

JNIEXPORT void JNICALL Java_org_maplibre_kmp_native_internal_Native_backendSetSize(JNIEnv* env, jclass, jlong backendPtr, jint width, jint height) {
    try {
        auto* backend = reinterpret_cast<MetalBackend2*>(backendPtr);
        backend->setSize(mbgl::Size{static_cast<uint32_t>(width), static_cast<uint32_t>(height)});
    } catch (const std::exception& e) {
        throwJavaException(env, "java/lang/RuntimeException", e.what());
    }
}

JNIEXPORT jlong JNICALL Java_org_maplibre_kmp_native_internal_Native_backendGetThreadPoolPtr(JNIEnv* env, jclass, jlong backendPtr) {
    try {
        auto* backend = reinterpret_cast<gfx::RendererBackend*>(backendPtr);
        return reinterpret_cast<jlong>(&backend->getThreadPool());
    } catch (const std::exception& e) {
        throwJavaException(env, "java/lang/RuntimeException", e.what());
        return 0;
    }
}

}
