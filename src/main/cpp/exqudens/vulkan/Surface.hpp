#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

    struct EXQUDENS_VULKAN_EXPORT Surface {

        class Builder;

        VkSurfaceKHR surface = nullptr;
        VULKAN_HPP_NAMESPACE::raii::SurfaceKHR target = nullptr;

        static Builder builder(Surface& object);

    };

    class EXQUDENS_VULKAN_EXPORT Surface::Builder {

        private:

            Surface& object;

        public:

            explicit Builder(Surface& object);

            Builder& setSurface(const VkSurfaceKHR& value);

            Surface& build(
                VULKAN_HPP_NAMESPACE::raii::Instance& instance
            );

    };
}

// implementation ---

#include <string>
#include <filesystem>

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

namespace exqudens::vulkan {

    EXQUDENS_VULKAN_INLINE Surface::Builder Surface::builder(Surface& object) {
        return Builder(object);
    }

    EXQUDENS_VULKAN_INLINE Surface::Builder::Builder(Surface& object): object(object) {
    }

    EXQUDENS_VULKAN_INLINE Surface::Builder& Surface::Builder::setSurface(const VkSurfaceKHR& value) {
        object.surface = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Surface& Surface::Builder::build(
        VULKAN_HPP_NAMESPACE::raii::Instance& instance
    ) {
        try {
            object.target = VULKAN_HPP_NAMESPACE::raii::SurfaceKHR(instance, object.surface);

            return object;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

}

#undef CALL_INFO
