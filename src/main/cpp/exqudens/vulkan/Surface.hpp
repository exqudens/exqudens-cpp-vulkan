#pragma once

#include <optional>
#include <memory>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

  struct EXQUDENS_VULKAN_EXPORT Surface {

    class Builder;

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES static Builder builder();

    std::shared_ptr<vk::raii::SurfaceKHR> value;

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES vk::raii::SurfaceKHR& reference();

  };

  class EXQUDENS_VULKAN_EXPORT Surface::Builder {

    private:

      std::weak_ptr<vk::raii::Instance> instance;
      VkSurfaceKHR vkSurface;

    public:

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Surface::Builder& setInstance(const std::weak_ptr<vk::raii::Instance>& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Surface::Builder& setVkSurface(const VkSurfaceKHR& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Surface build();

  };

}
