#ifndef __APPLE__

#include "jni_jawt_backend.hpp"
#include <stdexcept>

namespace maplibre_jni {

// Vulkan backend stub - to be implemented
class VulkanBackend final : public JAWTRendererBackend {
public:
    VulkanBackend(JNIEnv* env, jobject canvas, int width_, int height_)
        : JAWTRendererBackend(env, canvas, width_, height_) {
        throw std::runtime_error("Vulkan backend not yet implemented. "
                                "This is a placeholder for future Linux/Windows support.");
    }

    ~VulkanBackend() override = default;

    void updateSize(int width, int height) override {
        // Not implemented
    }

    void swap() override {
        // Not implemented
    }

    void* getRendererBackend() override {
        return nullptr;
    }
};

// Factory function implementation
std::unique_ptr<JAWTRendererBackend> createPlatformBackend(JNIEnv* env, jobject canvas, int width, int height) {
    return std::make_unique<VulkanBackend>(env, canvas, width, height);
}

} // namespace maplibre_jni

#endif // !__APPLE__