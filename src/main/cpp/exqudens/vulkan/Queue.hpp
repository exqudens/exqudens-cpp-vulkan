#pragma once

#include <optional>
#include <memory>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

  struct EXQUDENS_VULKAN_EXPORT Queue {

    class Builder;

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES static Builder builder();

    uint32_t familyIndex;
    uint32_t index;
    std::shared_ptr<vk::raii::Queue> value;

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES vk::raii::Queue& reference();

  };

  class EXQUDENS_VULKAN_EXPORT Queue::Builder {

    private:

      std::weak_ptr<vk::raii::Device> device;
      std::optional<uint32_t> familyIndex;
      std::optional<uint32_t> index;

    public:

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Queue::Builder& setDevice(const std::weak_ptr<vk::raii::Device>& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Queue::Builder& setFamilyIndex(const uint32_t& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Queue::Builder& setIndex(const uint32_t& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Queue build();

  };

}
