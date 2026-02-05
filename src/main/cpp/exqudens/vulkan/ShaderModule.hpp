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

        static Builder builder();

    };

    class EXQUDENS_VULKAN_EXPORT ShaderModule::Builder {

        private:

            ShaderModule resultObject = {};

        public:

            Builder& setFile(const std::optional<const char*>& value);

            Builder& setCode(const std::vector<char>& value);

            Builder& setReadFile(bool value);

            Builder& setCreateInfo(const VULKAN_HPP_NAMESPACE::ShaderModuleCreateInfo& value);

            ShaderModule& build(
                ShaderModule& shaderModule,
                VULKAN_HPP_NAMESPACE::raii::Device& device
            );

    };
}

// implementation ---

#include <cstdint>
#include <string>
#include <filesystem>
#include <fstream>

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

namespace exqudens::vulkan {

    EXQUDENS_VULKAN_INLINE ShaderModule::Builder ShaderModule::builder() {
        return {};
    }

    EXQUDENS_VULKAN_INLINE ShaderModule::Builder& ShaderModule::Builder::setFile(const std::optional<const char*>& value) {
        resultObject.file = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE ShaderModule::Builder& ShaderModule::Builder::setCode(const std::vector<char>& value) {
        resultObject.code = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE ShaderModule::Builder& ShaderModule::Builder::setReadFile(bool value) {
        resultObject.readFile = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE ShaderModule::Builder& ShaderModule::Builder::setCreateInfo(const VULKAN_HPP_NAMESPACE::ShaderModuleCreateInfo& value) {
        resultObject.createInfo = value;
        return *this;
    }

    EXQUDENS_VULKAN_INLINE ShaderModule& ShaderModule::Builder::build(
        ShaderModule& shaderModule,
        VULKAN_HPP_NAMESPACE::raii::Device& device
    ) {
        try {
            shaderModule.file = resultObject.file;
            shaderModule.code = resultObject.code;
            shaderModule.readFile = resultObject.readFile;

            if (shaderModule.readFile) {
                std::string filePath = shaderModule.file.value();
                std::ifstream fileStream(filePath, std::ios::ate | std::ios::binary);

                if (!fileStream.is_open()) {
                    throw std::runtime_error(CALL_INFO + ": failed to open file " + filePath);
                }

                std::vector<char> buffer(fileStream.tellg());
                fileStream.seekg(0, std::ios::beg);
                fileStream.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
                fileStream.close();
                shaderModule.code = buffer;
            }

            shaderModule.createInfo = resultObject.createInfo;
            shaderModule.createInfo.codeSize = shaderModule.code.size() * sizeof(char);
            shaderModule.createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderModule.code.data());
            shaderModule.target = device.createShaderModule(shaderModule.createInfo);

            return shaderModule;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

}

#undef CALL_INFO
