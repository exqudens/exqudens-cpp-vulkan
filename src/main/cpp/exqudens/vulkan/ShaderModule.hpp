#pragma once

#include <optional>
#include <vector>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

    struct EXQUDENS_VULKAN_EXPORT ShaderModule {

        class Builder;

        std::optional<const char*> file = {};
        std::vector<char> code = {};
        bool readFile = false;
        VULKAN_HPP_NAMESPACE::ShaderModuleCreateInfo createInfo;
        VULKAN_HPP_NAMESPACE::raii::ShaderModule target = nullptr;

        static Builder builder(ShaderModule& object);

    };

    class EXQUDENS_VULKAN_EXPORT ShaderModule::Builder {

        private:

            ShaderModule& object;

        public:

            explicit Builder(ShaderModule& object);

            Builder& setFile(const std::optional<const char*>& value);

            Builder& setCode(const std::vector<char>& value);

            Builder& setReadFile(bool value);

            Builder& setCreateInfo(const VULKAN_HPP_NAMESPACE::ShaderModuleCreateInfo& value);

            ShaderModule& build(
                VULKAN_HPP_NAMESPACE::raii::Device& device
            );

    };
}

// implementation ---

#include <cstdint>
#include <string>
#include <filesystem>
#include <stdexcept>
#include <fstream>

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

namespace exqudens::vulkan {

    EXQUDENS_VULKAN_INLINE ShaderModule::Builder ShaderModule::builder(ShaderModule& object) {
        return Builder(object);
    }

    EXQUDENS_VULKAN_INLINE ShaderModule::Builder::Builder(ShaderModule& object): object(object) {
    }

    EXQUDENS_VULKAN_INLINE ShaderModule::Builder& ShaderModule::Builder::setFile(const std::optional<const char*>& value) {
        object.file = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE ShaderModule::Builder& ShaderModule::Builder::setCode(const std::vector<char>& value) {
        object.code = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE ShaderModule::Builder& ShaderModule::Builder::setReadFile(bool value) {
        object.readFile = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE ShaderModule::Builder& ShaderModule::Builder::setCreateInfo(const VULKAN_HPP_NAMESPACE::ShaderModuleCreateInfo& value) {
        object.createInfo = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE ShaderModule& ShaderModule::Builder::build(
        VULKAN_HPP_NAMESPACE::raii::Device& device
    ) {
        try {
            if (object.readFile) {
                std::string filePath = object.file.value();
                std::ifstream fileStream(filePath, std::ios::ate | std::ios::binary);

                if (!fileStream.is_open()) {
                    throw std::runtime_error(CALL_INFO + ": failed to open file " + filePath);
                }

                std::vector<char> buffer(fileStream.tellg());
                fileStream.seekg(0, std::ios::beg);
                fileStream.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
                fileStream.close();
                object.code = buffer;
            }

            object.createInfo.codeSize = object.code.size() * sizeof(char);
            object.createInfo.pCode = reinterpret_cast<const uint32_t*>(object.code.data());
            object.target = device.createShaderModule(object.createInfo);

            return object;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

}

#undef CALL_INFO
