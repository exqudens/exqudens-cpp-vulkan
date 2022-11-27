#include "exqudens/vulkan/ImageView.hpp"
#include "exqudens/vulkan/macros.hpp"

#include <stdexcept>

namespace exqudens::vulkan {

  vk::raii::ImageView& ImageView::reference() {
    try {
      if (!value) {
        throw std::runtime_error(CALL_INFO() + ": value is not initialized!");
      }
      return *value;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO()));
    }
  }

  ImageView::Builder& ImageView::Builder::setDevice(const std::weak_ptr<vk::raii::Device>& val) {
    device = val;
    return *this;
  }

  ImageView::Builder& ImageView::Builder::setCreateInfo(const vk::ImageViewCreateInfo& val) {
    createInfo = val;
    return *this;
  }

  ImageView ImageView::Builder::build() {
    try {
      ImageView target = {};
      target.createInfo = createInfo.value();
      target.value = std::make_shared<vk::raii::ImageView>(
          *device.lock(),
          target.createInfo
      );
      return target;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO()));
    }
  }

  ImageView::Builder ImageView::builder() {
    return {};
  }

}
