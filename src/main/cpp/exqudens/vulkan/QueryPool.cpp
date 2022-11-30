#include "exqudens/vulkan/QueryPool.hpp"
#include "exqudens/vulkan/macros.hpp"

#include <stdexcept>

namespace exqudens::vulkan {

  vk::raii::QueryPool& QueryPool::reference() {
    try {
      if (!value) {
        throw std::runtime_error(CALL_INFO() + ": value is not initialized!");
      }
      return *value;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO()));
    }
  }

  QueryPool::Builder& QueryPool::Builder::setDevice(const std::weak_ptr<vk::raii::Device>& val) {
    device = val;
    return *this;
  }

  QueryPool::Builder& QueryPool::Builder::setCreateInfo(const vk::QueryPoolCreateInfo& val) {
    createInfo = val;
    return *this;
  }

  QueryPool QueryPool::Builder::build() {
    try {
      QueryPool target = {};
      target.createInfo = createInfo.value();
      target.value = std::make_shared<vk::raii::QueryPool>(
          *device.lock(),
          target.createInfo
      );
      return target;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO()));
    }
  }

  QueryPool::Builder QueryPool::builder() {
    return {};
  }

}
