#pragma once

#include <vulkan/vulkan_raii.hpp>

namespace exqudens::vulkan {

    struct Context {

        VULKAN_HPP_NAMESPACE::raii::Context target = {};

    };

}
