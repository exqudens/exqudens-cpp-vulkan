#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

    struct EXQUDENS_VULKAN_EXPORT Utils {

        static VULKAN_HPP_NAMESPACE::SwapchainCreateInfoKHR swapchainCreateInfo(
            VULKAN_HPP_NAMESPACE::raii::PhysicalDevice& physicalDevice,
            VULKAN_HPP_NAMESPACE::raii::SurfaceKHR& surface,
            int framebufferWidth,
            int framebufferHeight
        );

    };
}

// implementation ---

#include <cstdint>
#include <string>
#include <optional>
#include <vector>
#include <filesystem>

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

namespace exqudens::vulkan {

    EXQUDENS_VULKAN_INLINE VULKAN_HPP_NAMESPACE::SwapchainCreateInfoKHR Utils::swapchainCreateInfo(
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

}

#undef CALL_INFO
