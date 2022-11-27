#pragma once

#include <optional>
#include <memory>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

  struct EXQUDENS_VULKAN_EXPORT Semaphore {

    class Builder;

    EXQUDENS_VULKAN_INTERFACE_INLINE static Builder builder();

    vk::SemaphoreCreateInfo createInfo;
    std::shared_ptr<vk::raii::Semaphore> value;

    vk::raii::Semaphore& reference();

  };

  class EXQUDENS_VULKAN_EXPORT Semaphore::Builder {

    private:

      std::weak_ptr<vk::raii::Device> device;
      std::optional<vk::SemaphoreCreateInfo> createInfo;

    public:

      Semaphore::Builder& setDevice(const std::weak_ptr<vk::raii::Device>& val);

      Semaphore::Builder& setCreateInfo(const vk::SemaphoreCreateInfo& val);

      Semaphore build();

  };

}
