#pragma once

#include <optional>
#include <memory>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

  struct EXQUDENS_VULKAN_EXPORT Sampler {

    class Builder;

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES static Builder builder();

    vk::SamplerCreateInfo createInfo;
    std::shared_ptr<vk::raii::Sampler> value;

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES vk::raii::Sampler& reference();

  };

  class EXQUDENS_VULKAN_EXPORT Sampler::Builder {

    private:

      std::weak_ptr<vk::raii::Device> device;
      std::optional<vk::SamplerCreateInfo> createInfo;

    public:

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Sampler::Builder& setDevice(const std::weak_ptr<vk::raii::Device>& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Sampler::Builder& setCreateInfo(const vk::SamplerCreateInfo& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Sampler build();

  };

}
