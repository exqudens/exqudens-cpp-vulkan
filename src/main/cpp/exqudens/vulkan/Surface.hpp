#pragma once

#include <string>
#include <filesystem>

#include <vulkan/vulkan_raii.hpp>

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

namespace exqudens::vulkan {

    struct Surface {

        class Builder;

        VkSurfaceKHR surface = nullptr;
        VULKAN_HPP_NAMESPACE::raii::SurfaceKHR target = nullptr;

        static Builder builder();

    };

    class Surface::Builder {

        private:

            Surface resultObject = {};

        public:

            Builder& setSurface(const VkSurfaceKHR& value);

            Surface& build(
                Surface& surface,
                VULKAN_HPP_NAMESPACE::raii::Instance& instance
            );

    };

    // implementation ---

    inline Surface::Builder Surface::builder() {
        return {};
    }

    inline Surface::Builder& Surface::Builder::setSurface(const VkSurfaceKHR& value) {
        resultObject.surface = value;
        return *this;
    }

    inline Surface& Surface::Builder::build(
        Surface& surface,
        VULKAN_HPP_NAMESPACE::raii::Instance& instance
    ) {
        try {
            surface.surface = resultObject.surface;
            surface.target = VULKAN_HPP_NAMESPACE::raii::SurfaceKHR(instance, surface.surface);

            return surface;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }


}

#undef CALL_INFO
