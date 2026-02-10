#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

    struct EXQUDENS_VULKAN_EXPORT CommandBuffers {

        class Builder;

        VULKAN_HPP_NAMESPACE::CommandBufferAllocateInfo allocateInfo;
        VULKAN_HPP_NAMESPACE::raii::CommandBuffers targets = nullptr;

        static Builder builder(CommandBuffers& object);

    };

    class EXQUDENS_VULKAN_EXPORT CommandBuffers::Builder {

    private:

        CommandBuffers& object;

    public:

        explicit Builder(CommandBuffers& object);

        Builder& setAllocateInfo(const VULKAN_HPP_NAMESPACE::CommandBufferAllocateInfo& value);

        CommandBuffers& build(
            VULKAN_HPP_NAMESPACE::raii::Device& device
        );

    };

}

// implementation ---

#include <string>
#include <filesystem>
#include <stdexcept>

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

namespace exqudens::vulkan {

    EXQUDENS_VULKAN_INLINE CommandBuffers::Builder CommandBuffers::builder(CommandBuffers& object) {
        return Builder(object);
    }

    EXQUDENS_VULKAN_INLINE CommandBuffers::Builder::Builder(CommandBuffers& object): object(object) {
    }

    EXQUDENS_VULKAN_INLINE CommandBuffers::Builder& CommandBuffers::Builder::setAllocateInfo(const VULKAN_HPP_NAMESPACE::CommandBufferAllocateInfo& value) {
        object.allocateInfo = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE CommandBuffers& CommandBuffers::Builder::build(
        VULKAN_HPP_NAMESPACE::raii::Device& device
    ) {
        try {
            object.targets = VULKAN_HPP_NAMESPACE::raii::CommandBuffers(device, object.allocateInfo);

            return object;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

}

#undef CALL_INFO
