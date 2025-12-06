#pragma once

#include <vulkan/vulkan_raii.hpp>

namespace exqudens::vulkan {

    struct Context {

        VULKAN_HPP_NAMESPACE::raii::Context targetObject;

        VULKAN_HPP_NAMESPACE::raii::Context& get();

    };

// implementation ---

    inline VULKAN_HPP_NAMESPACE::raii::Context& Context::get() {
        return targetObject;
    }

}
