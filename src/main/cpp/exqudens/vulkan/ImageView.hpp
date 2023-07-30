#pragma once

#include <optional>
#include <memory>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

  struct EXQUDENS_VULKAN_EXPORT ImageView {

    class Builder;

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES static Builder builder();

    vk::ImageViewCreateInfo createInfo;
    std::shared_ptr<vk::raii::ImageView> value;

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES vk::raii::ImageView& reference();

  };

  class EXQUDENS_VULKAN_EXPORT ImageView::Builder {

    private:

      std::weak_ptr<vk::raii::Device> device;
      std::optional<vk::ImageViewCreateInfo> createInfo;

    public:

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES ImageView::Builder& setDevice(const std::weak_ptr<vk::raii::Device>& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES ImageView::Builder& setCreateInfo(const vk::ImageViewCreateInfo& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES ImageView build();

  };

}
