#pragma once

#include <optional>
#include <memory>
#include <stdexcept>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/macros.hpp"

namespace exqudens::vulkan {

  struct QueryPool {

    class Builder;

    inline static Builder builder();

    vk::QueryPoolCreateInfo createInfo;
    std::shared_ptr<vk::raii::QueryPool> value;

    vk::raii::QueryPool& reference() {
      try {
        if (!value) {
          throw std::runtime_error(CALL_INFO() + ": value is not initialized!");
        }
        return *value;
      } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO()));
      }
    }

  };

  class QueryPool::Builder {

    private:

      std::weak_ptr<vk::raii::Device> device;
      std::optional<vk::QueryPoolCreateInfo> createInfo;

    public:

      QueryPool::Builder& setDevice(const std::weak_ptr<vk::raii::Device>& val) {
        device = val;
        return *this;
      }

      QueryPool::Builder& setCreateInfo(const vk::QueryPoolCreateInfo& val) {
        createInfo = val;
        return *this;
      }

      QueryPool build() {
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

  };

  QueryPool::Builder QueryPool::builder() {
    return {};
  }

}
