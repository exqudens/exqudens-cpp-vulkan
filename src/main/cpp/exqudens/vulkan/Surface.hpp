#pragma once

#include <optional>
#include <memory>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

  struct EXQUDENS_VULKAN_EXPORT Surface {

    class Builder;

    EXQUDENS_VULKAN_INTERFACE_INLINE static Builder builder();

    std::shared_ptr<vk::raii::SurfaceKHR> value;

    vk::raii::SurfaceKHR& reference();

  };

  class EXQUDENS_VULKAN_EXPORT Surface::Builder {

    private:

      std::weak_ptr<vk::raii::Instance> instance;
      VkSurfaceKHR vkSurface;

    public:

      Surface::Builder& setInstance(const std::weak_ptr<vk::raii::Instance>& val);

      Surface::Builder& setVkSurface(const VkSurfaceKHR& val);

      Surface build();

  };

}
