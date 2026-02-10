#pragma once

#include <cstdint>
#include <vector>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

    struct EXQUDENS_VULKAN_EXPORT Swapchain {

        class Builder;

        std::vector<uint32_t> queueFamilyIndices = {};
        VULKAN_HPP_NAMESPACE::SwapchainCreateInfoKHR createInfo;
        VULKAN_HPP_NAMESPACE::raii::SwapchainKHR target = nullptr;

        static VULKAN_HPP_NAMESPACE::SwapchainCreateInfoKHR createInfoFrom(
            VULKAN_HPP_NAMESPACE::raii::PhysicalDevice& physicalDevice,
            VULKAN_HPP_NAMESPACE::raii::SurfaceKHR& surface,
            int framebufferWidth,
            int framebufferHeight
        );

        static Builder builder(Swapchain& object);

    };

    class EXQUDENS_VULKAN_EXPORT Swapchain::Builder {

        private:

            Swapchain& object;

        public:

            explicit Builder(Swapchain& object);

            Builder& setQueueFamilyIndices(const std::vector<uint32_t>& value);

            Builder& setCreateInfo(const VULKAN_HPP_NAMESPACE::SwapchainCreateInfoKHR& value);

            Swapchain& build(
                VULKAN_HPP_NAMESPACE::raii::Device& device
            );

    };
}

// implementation ---

#include <string>
#include <optional>
#include <algorithm>
#include <filesystem>
#include <stdexcept>

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

namespace exqudens::vulkan {

    EXQUDENS_VULKAN_INLINE VULKAN_HPP_NAMESPACE::SwapchainCreateInfoKHR Swapchain::createInfoFrom(
        VULKAN_HPP_NAMESPACE::raii::PhysicalDevice& physicalDevice,
        VULKAN_HPP_NAMESPACE::raii::SurfaceKHR& surface,
        int framebufferWidth,
        int framebufferHeight
    ) {
        try {
            std::optional<uint32_t> surfaceMinImageCount = {};
            std::optional<VULKAN_HPP_NAMESPACE::Extent2D> surfaceExtent = {};
            VULKAN_HPP_NAMESPACE::SurfaceCapabilitiesKHR surfaceCapabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);

            if (!surfaceMinImageCount.has_value()) {
                /*uint32_t minImageCount = surfaceCapabilities.minImageCount + 1;
                if (surfaceCapabilities.maxImageCount > 0 && minImageCount > surfaceCapabilities.maxImageCount) {
                    minImageCount = surfaceCapabilities.maxImageCount;
                }
                surfaceMinImageCount = minImageCount;*/
                uint32_t minImageCount = 3u > surfaceCapabilities.minImageCount ? 3u : surfaceCapabilities.minImageCount;
                if (0 < surfaceCapabilities.maxImageCount && surfaceCapabilities.maxImageCount < minImageCount) {
                    minImageCount = surfaceCapabilities.maxImageCount;
                }
                surfaceMinImageCount = minImageCount;
            }

            if (!surfaceExtent.has_value()) {
                if (surfaceCapabilities.currentExtent.width != 0xFFFFFFFF) {
                    surfaceExtent = surfaceCapabilities.currentExtent;
                } else {
                    surfaceExtent = VULKAN_HPP_NAMESPACE::Extent2D(
                        std::clamp<uint32_t>(framebufferWidth, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width),
                        std::clamp<uint32_t>(framebufferHeight, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height)
                    );
                }
            }

            if (!surfaceMinImageCount.has_value()) {
                throw std::runtime_error(CALL_INFO + ": 'surfaceMinImageCount' is not initialized");
            }

            if (!surfaceExtent.has_value()) {
                throw std::runtime_error(CALL_INFO + ": 'surfaceExtent' is not initialized");
            }

            std::optional<VULKAN_HPP_NAMESPACE::SurfaceFormatKHR> surfaceFormat = {};
            std::vector<VULKAN_HPP_NAMESPACE::SurfaceFormatKHR> surfaceFormats = physicalDevice.getSurfaceFormatsKHR(surface);

            if (surfaceFormats.empty()) {
                throw std::runtime_error(CALL_INFO + ": No surface formats available!");
            }

            if (!surfaceFormat.has_value()) {
                for (const auto& format : surfaceFormats) {
                    if (format.format == VULKAN_HPP_NAMESPACE::Format::eB8G8R8A8Srgb && format.colorSpace == VULKAN_HPP_NAMESPACE::ColorSpaceKHR::eSrgbNonlinear) {
                        surfaceFormat = format;
                    }
                }
                if (!surfaceFormat.has_value()) {
                    surfaceFormat = surfaceFormats.at(0);
                }
            }

            if (!surfaceFormat.has_value()) {
                throw std::runtime_error(CALL_INFO + ": 'surfaceFormat' is not initialized");
            }

            std::optional<VULKAN_HPP_NAMESPACE::PresentModeKHR> surfacePresentMode = {};
            std::vector<VULKAN_HPP_NAMESPACE::PresentModeKHR> surfacePresentModes = physicalDevice.getSurfacePresentModesKHR(surface);

            if (surfacePresentModes.empty()) {
                throw std::runtime_error(CALL_INFO + ": No surface present modes available!");
            }

            if (!surfacePresentMode.has_value()) {
                bool fifoFound = false;
                bool mailboxFound = false;
                for (const auto& mode : surfacePresentModes) {
                    if (!fifoFound && mode == VULKAN_HPP_NAMESPACE::PresentModeKHR::eFifo) {
                        fifoFound = true;
                    }
                    if (!mailboxFound && mode == VULKAN_HPP_NAMESPACE::PresentModeKHR::eMailbox) {
                        mailboxFound = true;
                    }
                    if (fifoFound && mailboxFound) {
                        break;
                    }
                }
                if (!fifoFound) {
                    throw std::runtime_error(CALL_INFO + ": Surface present mode fifo not found!");
                }
                surfacePresentMode = mailboxFound ? VULKAN_HPP_NAMESPACE::PresentModeKHR::eMailbox : VULKAN_HPP_NAMESPACE::PresentModeKHR::eFifo;
            }

            if (!surfacePresentMode.has_value()) {
                throw std::runtime_error(CALL_INFO + ": 'surfacePresentMode' is not initialized");
            }

            VULKAN_HPP_NAMESPACE::SwapchainCreateInfoKHR result;

            result.minImageCount = surfaceMinImageCount.value();
            result.imageExtent = surfaceExtent.value();
            result.preTransform = surfaceCapabilities.currentTransform;
            result.imageFormat = surfaceFormat.value().format;
            result.imageColorSpace = surfaceFormat.value().colorSpace;
            result.presentMode = surfacePresentMode.value();

            return result;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    EXQUDENS_VULKAN_INLINE Swapchain::Builder Swapchain::builder(Swapchain& object) {
        return Builder(object);
    }

    EXQUDENS_VULKAN_INLINE Swapchain::Builder::Builder(Swapchain& object): object(object) {
    }

    EXQUDENS_VULKAN_INLINE Swapchain::Builder& Swapchain::Builder::setQueueFamilyIndices(const std::vector<uint32_t>& value) {
        object.queueFamilyIndices = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Swapchain::Builder& Swapchain::Builder::setCreateInfo(const VULKAN_HPP_NAMESPACE::SwapchainCreateInfoKHR& value) {
        object.createInfo = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE Swapchain& Swapchain::Builder::build(
        VULKAN_HPP_NAMESPACE::raii::Device& device
    ) {
        try {
            if (object.queueFamilyIndices.size() > 2) {
                throw std::runtime_error(CALL_INFO + "Queue family indices size greater than 2");
            }

            if (object.queueFamilyIndices.size() == 2 && object.queueFamilyIndices.at(0) != object.queueFamilyIndices.at(1)) {
                object.createInfo.imageSharingMode = VULKAN_HPP_NAMESPACE::SharingMode::eConcurrent;
                object.createInfo.queueFamilyIndexCount = static_cast<uint32_t>(object.queueFamilyIndices.size());
                object.createInfo.pQueueFamilyIndices = object.queueFamilyIndices.data();
            } else {
                object.createInfo.imageSharingMode = VULKAN_HPP_NAMESPACE::SharingMode::eExclusive;
                object.createInfo.queueFamilyIndexCount = 0;
                object.createInfo.pQueueFamilyIndices = nullptr;
            }

            object.target = device.createSwapchainKHR(object.createInfo);

            return object;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

}

#undef CALL_INFO
