#pragma once

#include <optional>
#include <memory>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

  struct EXQUDENS_VULKAN_EXPORT Semaphore {

    class Builder;

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES static Builder builder();

    vk::SemaphoreCreateInfo createInfo;
    std::shared_ptr<vk::raii::Semaphore> value;

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES vk::raii::Semaphore& reference();

  };

  class EXQUDENS_VULKAN_EXPORT Semaphore::Builder {

    private:

      std::weak_ptr<vk::raii::Device> device;
      std::optional<vk::SemaphoreCreateInfo> createInfo;

    public:

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Semaphore::Builder& setDevice(const std::weak_ptr<vk::raii::Device>& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Semaphore::Builder& setCreateInfo(const vk::SemaphoreCreateInfo& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Semaphore build();

  };

}
