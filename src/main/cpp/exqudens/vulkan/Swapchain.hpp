#pragma once

#include <string>
#include <vector>
#include <filesystem>

#include <vulkan/vulkan_raii.hpp>

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

namespace exqudens::vulkan {

    struct Swapchain {

        class Builder;

        std::vector<uint32_t> queueFamilyIndices = {};
        VULKAN_HPP_NAMESPACE::SwapchainCreateInfoKHR createInfo;
        VULKAN_HPP_NAMESPACE::raii::SwapchainKHR target = nullptr;

        static Builder builder();

    };

    class Swapchain::Builder {

        private:

            Swapchain resultObject = {};

        public:

            Builder& setQueueFamilyIndices(const std::vector<uint32_t>& value);

            Builder& setCreateInfo(const VULKAN_HPP_NAMESPACE::SwapchainCreateInfoKHR& value);

            Swapchain& build(
                Swapchain& swapchain,
                VULKAN_HPP_NAMESPACE::raii::Device& device
            );

    };

    // implementation ---

    inline Swapchain::Builder Swapchain::builder() {
        return {};
    }

    inline Swapchain::Builder& Swapchain::Builder::setQueueFamilyIndices(const std::vector<uint32_t>& value) {
        resultObject.queueFamilyIndices = value;
        return *this;
    }


    inline Swapchain::Builder& Swapchain::Builder::setCreateInfo(const VULKAN_HPP_NAMESPACE::SwapchainCreateInfoKHR& value) {
        resultObject.createInfo = value;
        return *this;
    }

    inline Swapchain& Swapchain::Builder::build(
        Swapchain& swapchain,
        VULKAN_HPP_NAMESPACE::raii::Device& device
    ) {
        try {
            if (resultObject.queueFamilyIndices.size() > 2) {
                throw std::runtime_error(CALL_INFO + "Queue family indices size greater than 2");
            }

            swapchain.queueFamilyIndices = resultObject.queueFamilyIndices;
            swapchain.createInfo = resultObject.createInfo;

            if (swapchain.queueFamilyIndices.size() == 2 && swapchain.queueFamilyIndices.at(0) != swapchain.queueFamilyIndices.at(1)) {
                swapchain.createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
                swapchain.createInfo.queueFamilyIndexCount = static_cast<uint32_t>(swapchain.queueFamilyIndices.size());
                swapchain.createInfo.pQueueFamilyIndices = swapchain.queueFamilyIndices.data();
            } else {
                swapchain.createInfo.imageSharingMode = vk::SharingMode::eExclusive;
                swapchain.createInfo.queueFamilyIndexCount = 0;
                swapchain.createInfo.pQueueFamilyIndices = nullptr;
            }

            swapchain.target = device.createSwapchainKHR(resultObject.createInfo);

            return swapchain;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

}

#undef CALL_INFO
