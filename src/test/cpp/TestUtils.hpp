#pragma once

#include <cstdlib>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>

#include <vulkan/vulkan_raii.hpp>
#include <lodepng.h>
#ifndef TINYOBJLOADER_IMPLEMENTATION
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#endif

#include "TestMacros.hpp"
#include "TestConfiguration.hpp"
#include "exqudens/vulkan/all.hpp"
#include "exqudens/vulkan/Vertex.hpp"

class TestUtils {

  public:

    static std::string getExecutableFile() {
      try {
        return TestConfiguration::getExecutableFile();
      } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO()));
      }
    }

    static std::string getExecutableDir() {
      try {
        return TestConfiguration::getExecutableDir();
      } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO()));
      }
    }

    static std::optional<std::string> getEnvironmentVariable(const std::string& name) {
      try {
        std::optional<std::string> value;
#ifdef _WIN32
        char* buffer;
        size_t size;
        errno_t error = _dupenv_s(&buffer, &size, name.c_str());
        if (error) {
          return value;
        }
        if (buffer != nullptr) {
          value.emplace(std::string(buffer));
        }
#elif _WIN64
        char* buffer;
        size_t size;
        errno_t error = _dupenv_s(&buffer, &size, name.c_str());
        if (error) {
          return value;
        }
        if (buffer != nullptr) {
          value.emplace(std::string(buffer));
        }
#endif
        return value;
      } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO()));
      }
    }

    static void setEnvironmentVariable(const std::string& name, const std::string& value) {
      try {
#ifdef _WIN32
        _putenv_s(name.c_str(), value.c_str());
#elif _WIN64
        _putenv_s(name.c_str(), value.c_str());
#endif
      } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO()));
      }
    }

    static std::vector<std::string> toStringVector(
        const std::exception& exception,
        std::vector<std::string> previous = {}
    ) {
      previous.emplace_back(exception.what());
      try {
        std::rethrow_if_nested(exception);
        return previous;
      } catch (const std::exception& e) {
        return toStringVector(e, previous);
      } catch (...) {
        if (previous.empty()) {
          previous.emplace_back(CALL_INFO() + ": Empty stack!");
        }
        return previous;
      }
    }

    static std::vector<std::string> toStackTrace(const std::exception& exception) {
      try {
        std::vector<std::string> elements = toStringVector(exception);
        if (elements.size() > 1) {
          std::ranges::reverse(elements);
        }
        return elements;
      } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO()));
      }
    }

    static std::string toString(const std::exception& e) {
      try {
        std::vector<std::string> stackTrace = toStackTrace(e);
        std::ostringstream out;
        for (size_t i = 0; i < stackTrace.size(); i++) {
          out << stackTrace[i];
          if (i < stackTrace.size() - 1) {
            out << std::endl;
          }
        }
        return out.str();
      } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO()));
      }
    }

    static void readPng(
        const std::string& path,
        unsigned int& width,
        unsigned int& height,
        unsigned int& depth,
        std::vector<unsigned char>& data
    ) {
      try {
        unsigned int widthIn = 0;
        unsigned int heightIn = 0;
        unsigned int depthIn = 4;
        std::vector<unsigned char> dataIn;
        unsigned int error = lodepng::decode(dataIn, widthIn, heightIn, path);
        if (error) {
          throw std::runtime_error(
              CALL_INFO() + ": failed to read image '" + std::to_string(error) + "': " + lodepng_error_text(error)
          );
        }
        width = widthIn;
        height = heightIn;
        depth = depthIn;
        data = dataIn;
      } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO()));
      }
    }

    static std::vector<std::vector<std::vector<unsigned char>>> readPng(const std::string& path) {
      try {
        std::vector<std::vector<std::vector<unsigned char>>> image;
        std::vector<unsigned char> pixels;
        unsigned int width, height, depth;
        readPng(path, width, height, depth, pixels);
        unsigned int yMultiplier = width * depth;
        image.resize(height);
        for (unsigned int y = 0; y < height; y++) {
          image[y].resize(width);
          for (unsigned int x = 0; x < width; x++) {
            image[y][x].resize(depth);
            for (unsigned int z = 0; z < depth; z++) {
              unsigned int yOffset = y * yMultiplier;
              unsigned int xOffset = x * depth;
              unsigned int zOffset = z;
              unsigned int offset = yOffset + xOffset + zOffset;
              unsigned int i = offset;
              image[y][x][z] = pixels[i];
            }
          }
        }
        return image;
      } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO()));
      }
    }

    static void writePng(
        const std::string& path,
        unsigned int& width,
        unsigned int& height,
        unsigned int& depth,
        std::vector<unsigned char>& data
    ) {
      try {
        unsigned int widthIn = width;
        unsigned int heightIn = height;
        unsigned int depthIn = depth;
        unsigned int error = lodepng::encode(path, data, widthIn, heightIn);
        if (error) {
          throw std::runtime_error(
              CALL_INFO() + ": failed to write image '" + std::to_string(error) + "': " + lodepng_error_text(error)
          );
        }
      } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO()));
      }
    }

    static void writePng(const std::vector<std::vector<std::vector<unsigned char>>>& image, const std::string& path) {
      try {
        if (image.empty() || image[0].empty() || image[0][0].empty()) {
          throw std::runtime_error(
              CALL_INFO() + ": image.empty() || image[0].empty() || image[0][0].empty()"
          );
        }
        unsigned int height = image.size();
        unsigned int width = image[0].size();
        unsigned int depth = image[0][0].size();
        std::vector<unsigned char> pixels;
        pixels.resize(width * height * depth);
        unsigned int i = 0;
        for (unsigned int y = 0; y < height; y++) {
          for (unsigned int x = 0; x < width; x++) {
            for (unsigned int z = 0; z < depth; z++) {
              pixels[i] = image[y][x][z];
              i++;
            }
          }
        }
        writePng(path, width, height, depth, pixels);
      } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO()));
      }
    }

    static void readObj(
        const std::string& path,
        std::vector<exqudens::vulkan::Vertex>& vertices,
        std::vector<uint16_t>& indices
    ) {
      try {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, path.c_str())) {
          throw std::runtime_error(err);
        }

        std::unordered_map<exqudens::vulkan::Vertex, uint32_t> uniqueVertices{};

        for (const auto& shape: shapes) {
          for (const auto& index: shape.mesh.indices) {
            exqudens::vulkan::Vertex vertex = {};

            vertex.pos = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]
            };

            vertex.texCoord = {
                attrib.texcoords[2 * index.texcoord_index + 0],
                1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
            };

            vertex.color = {1.0f, 1.0f, 1.0f};

            if (uniqueVertices.count(vertex) == 0) {
              uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
              vertices.emplace_back(vertex);
            }

            indices.emplace_back(uniqueVertices[vertex]);
          }
        }
      } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO()));
      }
    }

    static vk::PipelineShaderStageCreateInfo createStage(
        exqudens::vulkan::Device& device,
        std::map<std::string, std::pair<vk::ShaderModuleCreateInfo, std::shared_ptr<vk::raii::ShaderModule>>>& shaders,
        const std::string& path
    ) {
      try {
        std::vector<char> bytes = exqudens::vulkan::Utility::readFile(path);
        if (bytes.empty()) {
          throw std::runtime_error(CALL_INFO() + ": '" + path + "' failed to create shader module bytes is empty!");
        }
        vk::ShaderModuleCreateInfo shaderCreateInfo = vk::ShaderModuleCreateInfo()
            .setCodeSize(bytes.size())
            .setPCode(reinterpret_cast<const uint32_t*>(bytes.data()));
        shaders[path] = std::make_pair(
            shaderCreateInfo,
            std::make_shared<vk::raii::ShaderModule>(device.reference(), shaderCreateInfo)
        );
        return vk::PipelineShaderStageCreateInfo()
            .setPName("main")
            .setModule(*(*shaders[path].second))
            .setStage(path.ends_with(".vert.spv") ? vk::ShaderStageFlagBits::eVertex : vk::ShaderStageFlagBits::eFragment);
      } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO()));
      }
    }

    static void copyTo(exqudens::vulkan::Buffer& buffer, const void* data) {
      try {
        void* tmpData = buffer.memoryReference().mapMemory(0, buffer.createInfo.size);
        std::memcpy(tmpData, data, static_cast<size_t>(buffer.createInfo.size));
        buffer.memoryReference().unmapMemory();
      } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO()));
      }
    }

    static void insertDepthImagePipelineBarrier(
        exqudens::vulkan::CommandBuffer& transferCommandBuffer,
        exqudens::vulkan::Image& depthImage
    ) {
      try {
        transferCommandBuffer.reference().pipelineBarrier(
            vk::PipelineStageFlagBits::eTopOfPipe,
            vk::PipelineStageFlagBits::eEarlyFragmentTests,
            vk::DependencyFlags(0),
            {},
            {},
            {
                vk::ImageMemoryBarrier()
                    .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                    .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                    .setImage(*depthImage.reference())
                    .setOldLayout(vk::ImageLayout::eUndefined)
                    .setNewLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
                    .setSrcAccessMask(vk::AccessFlagBits::eNoneKHR)
                    .setDstAccessMask(vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite)
                    .setSubresourceRange(
                        vk::ImageSubresourceRange()
                            .setAspectMask(
                                depthImage.createInfo.format == vk::Format::eD32SfloatS8Uint
                                || depthImage.createInfo.format == vk::Format::eD24UnormS8Uint
                                ? vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil
                                : vk::ImageAspectFlagBits::eDepth
                            )
                            .setBaseMipLevel(0)
                            .setLevelCount(1)
                            .setBaseArrayLayer(0)
                            .setLayerCount(1)
                    )
            }
        );
      } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO()));
      }
    }

    static void copyBufferToImageAndGenerateMipmaps(
        exqudens::vulkan::PhysicalDevice& physicalDevice,
        exqudens::vulkan::CommandBuffer& transferCommandBuffer,
        exqudens::vulkan::Buffer& textureBuffer,
        exqudens::vulkan::Image& textureImage
    ) {
      try {
        transferCommandBuffer.reference().pipelineBarrier(
            vk::PipelineStageFlagBits::eTopOfPipe,
            vk::PipelineStageFlagBits::eTransfer,
            vk::DependencyFlags(0),
            {},
            {},
            {
                vk::ImageMemoryBarrier()
                    .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                    .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                    .setImage(*textureImage.reference())
                    .setOldLayout(vk::ImageLayout::eUndefined)
                    .setNewLayout(vk::ImageLayout::eTransferDstOptimal)
                    .setSrcAccessMask(vk::AccessFlagBits::eNoneKHR)
                    .setDstAccessMask(vk::AccessFlagBits::eTransferWrite)
                    .setSubresourceRange(
                        vk::ImageSubresourceRange()
                            .setAspectMask(vk::ImageAspectFlagBits::eColor)
                            .setBaseMipLevel(0)
                            .setLevelCount(textureImage.createInfo.mipLevels)
                            .setBaseArrayLayer(0)
                            .setLayerCount(1)
                    )
            }
        );

        transferCommandBuffer.reference().copyBufferToImage(
            *textureBuffer.reference(),
            *textureImage.reference(),
            vk::ImageLayout::eTransferDstOptimal,
            {
                vk::BufferImageCopy()
                    .setBufferOffset(0)
                    .setBufferRowLength(0)
                    .setImageOffset(
                        vk::Offset3D()
                            .setX(0)
                            .setY(0)
                            .setZ(0)
                    )
                    .setImageExtent(
                        vk::Extent3D()
                            .setWidth(textureImage.createInfo.extent.width)
                            .setHeight(textureImage.createInfo.extent.height)
                            .setDepth(1)
                    )
                    .setImageSubresource(
                        vk::ImageSubresourceLayers()
                            .setAspectMask(vk::ImageAspectFlagBits::eColor)
                            .setMipLevel(0)
                            .setBaseArrayLayer(0)
                            .setLayerCount(1)
                    )
            }
        );

        // Check if image format supports linear blitting
        vk::FormatProperties formatProperties = physicalDevice.reference().getFormatProperties(textureImage.createInfo.format);

        if (!(formatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eSampledImageFilterLinear)) {
          throw std::runtime_error(CALL_INFO() + "texture image format does not support linear blitting!");
        }

        auto mipWidth = (int32_t) textureImage.createInfo.extent.width;
        auto mipHeight = (int32_t) textureImage.createInfo.extent.height;

        for (uint32_t i = 1; i < textureImage.createInfo.mipLevels; i++) {
          transferCommandBuffer.reference().pipelineBarrier(
              vk::PipelineStageFlagBits::eTransfer,
              vk::PipelineStageFlagBits::eTransfer,
              vk::DependencyFlags(0),
              {},
              {},
              {
                  vk::ImageMemoryBarrier()
                      .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                      .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                      .setImage(*textureImage.reference())
                      .setOldLayout(vk::ImageLayout::eTransferDstOptimal)
                      .setNewLayout(vk::ImageLayout::eTransferSrcOptimal)
                      .setSrcAccessMask(vk::AccessFlagBits::eTransferWrite)
                      .setDstAccessMask(vk::AccessFlagBits::eTransferRead)
                      .setSubresourceRange(
                          vk::ImageSubresourceRange()
                              .setAspectMask(vk::ImageAspectFlagBits::eColor)
                              .setBaseMipLevel(i - 1)
                              .setLevelCount(1)
                              .setBaseArrayLayer(0)
                              .setLayerCount(1)
                      )
              }
          );

          vk::ImageBlit blit = vk::ImageBlit()
              .setSrcOffsets({
                                 vk::Offset3D()
                                     .setX(0)
                                     .setY(0)
                                     .setZ(0),
                                 vk::Offset3D()
                                     .setX(mipWidth)
                                     .setY(mipHeight)
                                     .setZ(1)
                             })
              .setSrcSubresource(
                  vk::ImageSubresourceLayers()
                      .setLayerCount(1)
                      .setLayerCount(1)
                      .setAspectMask(vk::ImageAspectFlagBits::eColor)
                      .setLayerCount(1)
                      .setMipLevel(i - 1)
                      .setBaseArrayLayer(0)
                      .setLayerCount(1)
              )
              .setDstOffsets({
                                 vk::Offset3D()
                                     .setX(0)
                                     .setY(0)
                                     .setZ(0),
                                 vk::Offset3D()
                                     .setX((int32_t) (mipWidth > 1 ? mipWidth / 2 : 1))
                                     .setY((int32_t) (mipHeight > 1 ? mipHeight / 2 : 1))
                                     .setZ((int32_t) 1)
                             })
              .setDstSubresource(
                  vk::ImageSubresourceLayers()
                      .setAspectMask(vk::ImageAspectFlagBits::eColor)
                      .setMipLevel(i)
                      .setBaseArrayLayer(0)
                      .setLayerCount(1)
              );

          transferCommandBuffer.reference().blitImage(
              *textureImage.reference(),
              vk::ImageLayout::eTransferSrcOptimal,
              *textureImage.reference(),
              vk::ImageLayout::eTransferDstOptimal,
              blit,
              vk::Filter::eLinear
          );

          transferCommandBuffer.reference().pipelineBarrier(
              vk::PipelineStageFlagBits::eTransfer,
              vk::PipelineStageFlagBits::eFragmentShader,
              vk::DependencyFlags(0),
              {},
              {},
              {
                  vk::ImageMemoryBarrier()
                      .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                      .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                      .setImage(*textureImage.reference())
                      .setOldLayout(vk::ImageLayout::eTransferSrcOptimal)
                      .setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
                      .setSrcAccessMask(vk::AccessFlagBits::eTransferRead)
                      .setDstAccessMask(vk::AccessFlagBits::eShaderRead)
                      .setSubresourceRange(
                          vk::ImageSubresourceRange()
                              .setAspectMask(vk::ImageAspectFlagBits::eColor)
                              .setBaseMipLevel(i - 1)
                              .setLevelCount(1)
                              .setBaseArrayLayer(0)
                              .setLayerCount(1)
                      )
              }
          );

          if (mipWidth > 1) mipWidth /= 2;
          if (mipHeight > 1) mipHeight /= 2;
        }

        transferCommandBuffer.reference().pipelineBarrier(
            vk::PipelineStageFlagBits::eTransfer,
            vk::PipelineStageFlagBits::eFragmentShader,
            vk::DependencyFlags(0),
            {},
            {},
            {
                vk::ImageMemoryBarrier()
                    .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                    .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                    .setImage(*textureImage.reference())
                    .setOldLayout(vk::ImageLayout::eTransferDstOptimal)
                    .setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
                    .setSrcAccessMask(vk::AccessFlagBits::eTransferWrite)
                    .setDstAccessMask(vk::AccessFlagBits::eShaderRead)
                    .setSubresourceRange(
                        vk::ImageSubresourceRange()
                            .setAspectMask(vk::ImageAspectFlagBits::eColor)
                            .setBaseMipLevel(textureImage.createInfo.mipLevels - 1)
                            .setLevelCount(1)
                            .setBaseArrayLayer(0)
                            .setLayerCount(1)
                    )
            }
        );
      } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO()));
      }
    }

};
