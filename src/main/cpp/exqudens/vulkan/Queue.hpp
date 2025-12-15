#pragma once

#include <cstdint>
#include <string>
#include <optional>
#include <filesystem>

#include <vulkan/vulkan_raii.hpp>

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

namespace exqudens::vulkan {

    struct Queue {

        class Builder;

        std::optional<uint32_t> familyIndex = {};
        std::optional<uint32_t> index = {};
        VULKAN_HPP_NAMESPACE::raii::Queue target = nullptr;

        static Builder builder();

    };

    class Queue::Builder {

        private:

            Queue resultObject = {};

        public:

            Builder& setFamilyIndex(const std::optional<uint32_t>& value);

            Builder& setIndex(const std::optional<uint32_t>& value);

            Queue& build(
                Queue& queue,
                VULKAN_HPP_NAMESPACE::raii::Device& device
            );

    };

    // implementation ---

    inline Queue::Builder Queue::builder() {
        return {};
    }

    inline Queue::Builder& Queue::Builder::setFamilyIndex(const std::optional<uint32_t>& value) {
        resultObject.familyIndex = value;
        return *this;
    }

    inline Queue::Builder& Queue::Builder::setIndex(const std::optional<uint32_t>& value) {
        resultObject.index = value;
        return *this;
    }

    inline Queue& Queue::Builder::build(
        Queue& queue,
        VULKAN_HPP_NAMESPACE::raii::Device& device
    ) {
        try {
            if (!resultObject.familyIndex.has_value()) {
                throw std::runtime_error(CALL_INFO + ": family index not set");
            }

            if (!resultObject.index.has_value()) {
                resultObject.index = 0;
            }

            queue.familyIndex = resultObject.familyIndex;
            queue.index = resultObject.index;
            queue.target = device.getQueue(queue.familyIndex.value(), queue.index.value());

            return queue;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }


}

#undef CALL_INFO
