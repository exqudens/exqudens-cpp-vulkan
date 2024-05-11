#include <cstdlib>
#include <string>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <functional>
#include <stdexcept>

#include <lodepng.h>
#include <vulkan/vulkan_raii.hpp>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "TestUtils.hpp"
#include "TestConfiguration.hpp"
#include "exqudens/vulkan/UniformBufferObject.hpp"

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

std::string TestUtils::getExecutableFile() {
  try {
    return TestConfiguration::getExecutableFile();
  } catch (...) {
    std::throw_with_nested(std::runtime_error(CALL_INFO));
  }
}

std::string TestUtils::getExecutableDir() {
  try {
    return TestConfiguration::getExecutableDir();
  } catch (...) {
    std::throw_with_nested(std::runtime_error(CALL_INFO));
  }
}

std::optional<std::string> TestUtils::getEnvironmentVariable(const std::string& name) {
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
    std::throw_with_nested(std::runtime_error(CALL_INFO));
  }
}

void TestUtils::setEnvironmentVariable(const std::string& name, const std::string& value) {
  try {
#ifdef _WIN32
    _putenv_s(name.c_str(), value.c_str());
#elif _WIN64
    _putenv_s(name.c_str(), value.c_str());
#endif
  } catch (...) {
    std::throw_with_nested(std::runtime_error(CALL_INFO));
  }
}

std::vector<std::string> TestUtils::toStringVector(
    const std::exception& exception,
    std::vector<std::string> previous
) {
  previous.emplace_back(exception.what());
  try {
    std::rethrow_if_nested(exception);
    return previous;
  } catch (const std::exception& e) {
    return toStringVector(e, previous);
  } catch (...) {
    if (previous.empty()) {
      previous.emplace_back(CALL_INFO + ": Empty stack!");
    }
    return previous;
  }
}

std::vector<std::string> TestUtils::toStackTrace(const std::exception& exception) {
  try {
    std::vector<std::string> elements = toStringVector(exception);
    if (elements.size() > 1) {
      std::ranges::reverse(elements);
    }
    return elements;
  } catch (...) {
    std::throw_with_nested(std::runtime_error(CALL_INFO));
  }
}

std::string TestUtils::toString(const std::exception& e) {
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
    std::throw_with_nested(std::runtime_error(CALL_INFO));
  }
}

void TestUtils::readPng(
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
          CALL_INFO + ": failed to read image '" + std::to_string(error) + "': " + lodepng_error_text(error) + ": '" + path + "'"
      );
    }
    width = widthIn;
    height = heightIn;
    depth = depthIn;
    data = dataIn;
  } catch (...) {
    std::throw_with_nested(std::runtime_error(CALL_INFO));
  }
}

std::vector<std::vector<std::vector<unsigned char>>> TestUtils::readPng(const std::string& path) {
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
    std::throw_with_nested(std::runtime_error(CALL_INFO));
  }
}

void TestUtils::writePng(
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
          CALL_INFO + ": failed to write image '" + std::to_string(error) + "': " + lodepng_error_text(error) + ": '" + path + "'"
      );
    }
  } catch (...) {
    std::throw_with_nested(std::runtime_error(CALL_INFO));
  }
}

void TestUtils::writePng(const std::vector<std::vector<std::vector<unsigned char>>>& image, const std::string& path) {
  try {
    if (image.empty() || image[0].empty() || image[0][0].empty()) {
      throw std::runtime_error(
          CALL_INFO + ": image.empty() || image[0].empty() || image[0][0].empty()"
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
    std::throw_with_nested(std::runtime_error(CALL_INFO));
  }
}

void TestUtils::readObj(
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

        if (index.vertex_index >= 0) {
          vertex.position = {
              attrib.vertices[3 * index.vertex_index + 0],
              attrib.vertices[3 * index.vertex_index + 1],
              attrib.vertices[3 * index.vertex_index + 2]
          };
        }

        if (index.normal_index >= 0) {
          vertex.normal = {
              attrib.normals[3 * index.normal_index + 0],
              attrib.normals[3 * index.normal_index + 1],
              attrib.normals[3 * index.normal_index + 2]
          };
        }

        if (index.texcoord_index >= 0) {
          vertex.texCoord = {
              attrib.texcoords[2 * index.texcoord_index + 0],
              1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
          };
        }

        vertex.color = {1.0f, 1.0f, 1.0f};

        if (uniqueVertices.count(vertex) == 0) {
          uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
          vertices.emplace_back(vertex);
        }

        indices.emplace_back(uniqueVertices[vertex]);
      }
    }
  } catch (...) {
    std::throw_with_nested(std::runtime_error(CALL_INFO));
  }
}

vk::PipelineShaderStageCreateInfo TestUtils::createStage(
    exqudens::vulkan::Device& device,
    std::map<std::string, std::pair<vk::ShaderModuleCreateInfo, std::shared_ptr<vk::raii::ShaderModule>>>& shaders,
    const std::string& path
) {
  try {
    std::vector<char> bytes = exqudens::vulkan::Utility::readFileBytes(path);
    if (bytes.empty()) {
      throw std::runtime_error(CALL_INFO + ": '" + path + "' failed to create shader module bytes is empty!");
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
    std::throw_with_nested(std::runtime_error(CALL_INFO));
  }
}

void TestUtils::insertDepthImagePipelineBarrier(
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
    std::throw_with_nested(std::runtime_error(CALL_INFO));
  }
}

void TestUtils::copyBufferToImageAndGenerateMipmaps(
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
                .setImageExtent(textureImage.createInfo.extent)
                .setImageSubresource(
                    vk::ImageSubresourceLayers()
                        .setAspectMask(vk::ImageAspectFlagBits::eColor)
                        .setMipLevel(0)
                        .setBaseArrayLayer(0)
                        .setLayerCount(1)
                )
        }
    );

    if (textureImage.createInfo.mipLevels > 1) {
      // Check if image format supports linear blitting
      vk::FormatProperties formatProperties = physicalDevice.reference().getFormatProperties(textureImage.createInfo.format);

      if (!(formatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eSampledImageFilterLinear)) {
        throw std::runtime_error(CALL_INFO + "texture image format does not support linear blitting!");
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
    std::throw_with_nested(std::runtime_error(CALL_INFO));
  }
}

void TestUtils::updateUniformBuffer(
    exqudens::vulkan::Buffer& shadowUniformBuffer,
    exqudens::vulkan::Buffer& uniformBuffer,
    std::chrono::time_point<std::chrono::high_resolution_clock>& startTime,
    float& angleLeft,
    float& angleUp,
    const uint32_t& width,
    const uint32_t& height,
    const std::string& animate,
    const bool& left,
    const bool& right,
    const bool& up,
    const bool& down
) {
  try {
    float angle1 = 0.0f;
    glm::vec3 axis1 = glm::vec3(0.0f, 0.0f, 1.0f);

    float angle2 = 0.0f;
    glm::vec3 axis2 = glm::vec3(0.0f, 1.0f, 0.0f);

    if (animate == std::string("auto")) {
      auto currentTime = std::chrono::high_resolution_clock::now();
      float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
      angle1 = time * glm::radians(90.0f);
      angle2 = 0.0f;
    } else if(animate == std::string("control")) {
      if (left) {
        float tmpMod = angleLeft + 0.01f;
        if (tmpMod >= 1.0f) {
          tmpMod = 0.0f;
        }
        angleLeft = tmpMod;
      } else if (right) {
        float tmpMod = angleLeft - 0.01f;
        if (tmpMod <= 0.0f) {
          tmpMod = 1.0f;
        }
        angleLeft = tmpMod;
      }

      angle1 = angleLeft * glm::radians(360.0f);

      if (up) {
        float tmpMod = angleUp + 0.01f;
        if (tmpMod >= 1.0f) {
          tmpMod = 0.0f;
        }
        angleUp = tmpMod;
      } else if (down) {
        float tmpMod = angleUp - 0.01f;
        if (tmpMod <= 0.0f) {
          tmpMod = 1.0f;
        }
        angleUp = tmpMod;
      }

      angle2 = angleUp * glm::radians(360.0f);
    } else {
      angle1 = 0.0f * glm::radians(360.0f); // min 0 max 360
      angle2 = 0.0f;
    }

    exqudens::vulkan::UniformBufferObject ubo = {};

    ubo.model = glm::rotate(glm::mat4(1.0f), angle1, axis1);
    ubo.model = glm::rotate(ubo.model, angle2, axis2);
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(
        glm::radians(45.0f),
        (float) width / (float) height,
        0.1f,
        10.0f
    );
    ubo.proj[1][1] *= -1;

    // ---
    glm::vec3 lightPos = glm::vec3();
    float lightFOV = 45.0f;
    float zNear = 1.0f;
    float zFar = 96.0f;

    // Animate the light source
    auto currentTime = std::chrono::high_resolution_clock::now();
    float timer = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count() / 4;
    lightPos.x = cos(glm::radians(timer * 360.0f)) * 40.0f;
    lightPos.y = -50.0f + sin(glm::radians(timer * 360.0f)) * 20.0f;
    lightPos.z = 25.0f + sin(glm::radians(timer * 360.0f)) * 5.0f;

    glm::mat4 depthProjectionMatrix = glm::perspective(glm::radians(lightFOV), 1.0f, zNear, zFar);
    glm::mat4 depthViewMatrix = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0, 1, 0));
    glm::mat4 depthModelMatrix = glm::mat4(1.0f);

    exqudens::vulkan::UniformBufferObject shadowUbo = {};

    shadowUbo.lightSpace = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;

    ubo.lightPos = glm::vec4(lightPos, 1.0f);
    ubo.lightSpace = shadowUbo.lightSpace;
    ubo.zNear = zNear;
    ubo.zFar = zFar;
    // ---

    shadowUniformBuffer.fill(&shadowUbo);
    uniformBuffer.fill(&ubo);
  } catch (...) {
    std::throw_with_nested(std::runtime_error(CALL_INFO));
  }
}

void TestUtils::copySwapchainImageToOutputImage(
    exqudens::vulkan::CommandBuffer& transferCommandBuffer,
    exqudens::vulkan::Image& swapchainImage,
    exqudens::vulkan::Image& outputImage,
    exqudens::vulkan::Queue& transferQueue
) {
  try {
    transferCommandBuffer.reference().begin({});

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
                .setImage(*outputImage.reference())
                .setOldLayout(vk::ImageLayout::eUndefined)
                .setNewLayout(vk::ImageLayout::eTransferDstOptimal)
                .setSrcAccessMask(vk::AccessFlagBits::eNone)
                .setDstAccessMask(vk::AccessFlagBits::eTransferWrite)
                .setSubresourceRange(
                    vk::ImageSubresourceRange()
                        .setAspectMask(vk::ImageAspectFlagBits::eColor)
                        .setBaseMipLevel(0)
                        .setLevelCount(1)
                        .setBaseArrayLayer(0)
                        .setLayerCount(1)
                )
        }
    );

    transferCommandBuffer.reference().copyImage(
        *swapchainImage.reference(),
        vk::ImageLayout::eTransferSrcOptimal,
        *outputImage.reference(),
        vk::ImageLayout::eTransferDstOptimal,
        {
            vk::ImageCopy()
                .setExtent(outputImage.createInfo.extent)
                .setSrcSubresource(
                    vk::ImageSubresourceLayers()
                        .setAspectMask(vk::ImageAspectFlagBits::eColor)
                        .setLayerCount(1)
                )
                .setDstSubresource(
                    vk::ImageSubresourceLayers()
                        .setAspectMask(vk::ImageAspectFlagBits::eColor)
                        .setLayerCount(1)
                )
        }
    );

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
                .setImage(*outputImage.reference())
                .setOldLayout(vk::ImageLayout::eTransferDstOptimal)
                .setNewLayout(vk::ImageLayout::eGeneral)
                .setSrcAccessMask(vk::AccessFlagBits::eTransferWrite)
                .setDstAccessMask(vk::AccessFlagBits::eMemoryRead)
                .setSubresourceRange(
                    vk::ImageSubresourceRange()
                        .setAspectMask(vk::ImageAspectFlagBits::eColor)
                        .setBaseMipLevel(0)
                        .setLevelCount(1)
                        .setBaseArrayLayer(0)
                        .setLayerCount(1)
                )
        }
    );

    transferCommandBuffer.reference().end();
    transferQueue.reference().submit(
        {
            vk::SubmitInfo()
                .setCommandBufferCount(1)
                .setPCommandBuffers(&(*transferCommandBuffer.reference()))
        }
    );
    transferQueue.reference().waitIdle();
  } catch (...) {
    std::throw_with_nested(std::runtime_error(CALL_INFO));
  }
}

#undef CALL_INFO
