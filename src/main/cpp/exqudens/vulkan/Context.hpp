#pragma once

#include <string>
#include <filesystem>
#include <memory>
#include <stdexcept>

#include <vulkan/vulkan_raii.hpp>

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

namespace exqudens::vulkan {

    struct Context {

        class Builder;

        std::shared_ptr<VULKAN_HPP_NAMESPACE::raii::Context> targetObject;

        inline static Builder builder();

        inline std::shared_ptr<VULKAN_HPP_NAMESPACE::raii::Context> getPtr();

        inline VULKAN_HPP_NAMESPACE::raii::Context& getRaii();

    };

    class Context::Builder {

        private:

            Context resultObject = {};

        public:

            inline Context build();

    };

    // implementation ---

    Context::Builder Context::builder() {
        return {};
    }

    std::shared_ptr<VULKAN_HPP_NAMESPACE::raii::Context> Context::getPtr() {
        return targetObject;
    }

    VULKAN_HPP_NAMESPACE::raii::Context& Context::getRaii() {
        try {
            if (!targetObject) {
                throw std::runtime_error(CALL_INFO + ": target object is not initialized!");
            }
            return *targetObject;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    Context Context::Builder::build() {
        try {
            Context result = resultObject;
            result.targetObject = std::make_shared<VULKAN_HPP_NAMESPACE::raii::Context>();
            return result;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

}

#undef CALL_INFO
