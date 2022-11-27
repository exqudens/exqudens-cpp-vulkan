#include "exqudens/vulkan/Sampler.hpp"
#include "exqudens/vulkan/macros.hpp"

#include <stdexcept>

namespace exqudens::vulkan {

  vk::raii::Sampler& Sampler::reference() {
    try {
      if (!value) {
        throw std::runtime_error(CALL_INFO() + ": value is not initialized!");
      }
      return *value;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO()));
    }
  }

  Sampler::Builder& Sampler::Builder::setDevice(const std::weak_ptr<vk::raii::Device>& val) {
    device = val;
    return *this;
  }

  Sampler::Builder& Sampler::Builder::setCreateInfo(const vk::SamplerCreateInfo& val) {
    createInfo = val;
    return *this;
  }

  Sampler Sampler::Builder::build() {
    try {
      Sampler target = {};
      target.createInfo = createInfo.value();
      target.value = std::make_shared<vk::raii::Sampler>(
          *device.lock(),
          target.createInfo
      );
      return target;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO()));
    }
  }

  Sampler::Builder Sampler::builder() {
    return {};
  }

}
